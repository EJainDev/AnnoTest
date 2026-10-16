module;

#include <cstdio>

#include "annotest_platform.h"

module annotest;

import std;

namespace annotest {

void set_fixture_cleanup(execution_suite& output, std::shared_ptr<void> fixture,
                         void (*cleanup)(void*), std::shared_ptr<suite_hook_state> hooks) {
  output.finish_fixture = [fixture = std::move(fixture), cleanup, hooks = std::move(hooks)] {
    cleanup(fixture.get());
    finish_suite_hooks(hooks);
  };
}

suite_builder::suite_builder(std::string name, std::string file)
    : suite{.name = std::move(name), .file = std::move(file)} {}

suite_builder::suite_builder(std::string_view prefix, std::string_view name,
                             std::string_view suffix, std::string_view file)
    : suite{.file = std::string(file)} {
  suite.name.reserve(prefix.size() + name.size() + suffix.size());
  suite.name.append(prefix);
  suite.name.append(name);
  suite.name.append(suffix);
}

suite_builder::~suite_builder() = default;

void commit_suite(std::vector<execution_suite>& suites, execution_suite& suite) {
  validate_case_names(suite);
  suites.push_back(std::move(suite));
}

std::string format_runtime(std::string_view text, std::format_args values) {
  return std::vformat(text, values);
}

int report_runner_error(const std::exception& error) {
  std::println(std::cerr, "Cannot run tests: {}", error.what());
  return 1;
}

void add_failure(std::string_view msg, const std::source_location& loc) {
  suite_log.add_failure(failure_xml_log{
      failure_types::assertion, std::string{msg},
      failure_cdata{std::make_pair(loc.file_name(), loc.line()), std::string{msg}}});
  throw assertion_error{std::string{msg}, true};
}

void add_failure_expect(std::string_view msg, const std::source_location& loc) {
  suite_log.add_failure(failure_xml_log{
      failure_types::expectation, std::string{msg},
      failure_cdata{std::make_pair(loc.file_name(), loc.line()), std::string{msg}}});
}

void numeric_equality_failure(std::format_args values, const std::source_location& location,
                              bool equal) {
  add_failure(format_runtime(equal ? "Assertion failed: expected {}, got {}"
                                   : "Assertion failed: expected not equal to {}, got {}",
                             values),
              location);
}

void record_property(std::string name, std::string value) {
  if (!active_context || suite_log.testcases.empty()) throw std::logic_error("No active test");
  suite_log.testcases.back().properties.push_back({std::move(name), std::move(value)});
}

std::string parameter_case_name(std::string_view base, int index) {
  return std::string(base) + "." + std::to_string(index + 1);
}

std::string format_parameter_list(std::span<const std::string> fields) {
  std::string result = "(";
  for (std::size_t index = 0; index < fields.size(); ++index) {
    if (index) result += ", ";
    result += fields[index];
  }
  return result + ")";
}

void delete_fixture_context(test_context* context) noexcept { delete context; }

fixture_context make_fixture_context() { return {new test_context, delete_fixture_context}; }

std::shared_ptr<suite_hook_state> make_suite_hooks_runtime(
    const std::string& name, const std::vector<std::string>& tags,
    std::shared_ptr<suite_hook_state> parent,
    static_tags labels, bool has_hooks, void (*before)(), void (*after)()) {
  auto state = std::make_shared<suite_hook_state>();
  state->parent = std::move(parent);
  state->has_hooks = has_hooks;
  state->context.suite = name;
  state->context.name = "_suite";
  state->context.tags = tags;
  for (auto label : labels) state->context.tags.emplace_back(label);
  state->before = before;
  state->after = after;
  return state;
}

execution_case make_execution_case(const discovered_test& description, static_tags tags,
                                   const std::source_location& location, bool disabled,
                                   double timeout, unsigned repetitions,
                                   const std::vector<std::string>& inherited_tags,
                                   const execution_conditions& conditions) {
  execution_case entry{.name = description.name,
                       .base_name = description.name,
                       .file = location.file_name(),
                       .line = location.line(),
                       .tags = inherited_tags,
                       .disabled = disabled,
                       .timeout_seconds = timeout,
                       .repetitions = repetitions,
                       .conditions = conditions};
  for (auto label : tags) entry.tags.emplace_back(label);
#if defined(_WIN32)
  constexpr auto os = operating_system::windows;
#elif defined(__APPLE__)
  constexpr auto os = operating_system::macos;
#else
  constexpr auto os = operating_system::linux_os;
#endif
  if (description.has_required_os &&
      std::ranges::find(description.required_os, os) == description.required_os.end())
    entry.skip_reason = "Required operating system unavailable";
  if (std::ranges::find(description.disallowed_os, os) != description.disallowed_os.end())
    entry.skip_reason = "Operating system disallowed";
  return entry;
}

void append_static_case(execution_suite& output, execution_case entry,
                        std::shared_ptr<void> fixture, int (*run)(void*, int), int index,
                        std::string parameters) {
  if (index >= 0) entry.name += "." + std::to_string(index + 1);
  entry.parameters = std::move(parameters);
  entry.run = [fixture = std::move(fixture), run, index] { return run(fixture.get(), index); };
  output.cases.push_back(std::move(entry));
}

void append_regular_case(execution_suite& output, execution_case entry,
                         std::shared_ptr<void> fixture, const discovered_test& test) {
  entry.run = [fixture = std::move(fixture), execute = test.execute,
               body = test.body, name = entry.base_name] {
    return execute(fixture.get(), name, body);
  };
  output.cases.push_back(std::move(entry));
}

void append_static_cases(execution_suite& output, std::shared_ptr<void> fixture,
                         std::span<const static_test_description> descriptions,
                         bool parent_disabled, const std::vector<std::string>& inherited_tags,
                         const execution_conditions& conditions) {
  for (const auto& description : descriptions) {
    const auto& test = description.test;
    auto entry = make_execution_case(test, description.tags, description.location,
                                     test.disabled || parent_disabled, description.timeout_seconds,
                                     description.repetitions, inherited_tags, conditions);
    if (description.conditions) description.conditions(entry.conditions);
    if (test.parameterized) {
      for (int index = 0; index < test.parameter_count; ++index)
        append_static_case(output, entry, fixture, test.run, index, description.parameters(index));
    } else {
      append_regular_case(output, std::move(entry), fixture, test);
    }
  }
}

void validate_case_names(const execution_suite& suite) {
  std::unordered_set<std::string> names;
  for (const auto& entry : suite.cases)
    if (!names.insert(entry.name).second)
      throw std::logic_error("Duplicate expanded test name: " + entry.name);
}

int run_with_suites(int argc, char** argv, void (*discover)(std::vector<execution_suite>&)) {
  auto options = parse_options(argc, argv);
  if (options.should_exit) return options.exit_code;
  try {
    std::vector<execution_suite> suites;
    if (discover) discover(suites);
    append_registered_tests(suites);
    return run_plan(std::move(suites), options, argc, argv);
  } catch (const std::exception& error) {
    return report_runner_error(error);
  }
}

int run_registered_tests(int argc, char** argv) { return run_with_suites(argc, argv, nullptr); }

void append_registered_tests(std::vector<execution_suite>& suites) {
  std::unordered_map<std::string,
                     std::unordered_map<std::type_index, std::shared_ptr<suite_hook_state>>>
      hooks;
  for (const auto& suite : suites)
    if (suite.suite_hooks) hooks[suite.name][suite.fixture_type] = suite.suite_hooks;
  for (const auto& test : registered_cases) {
    auto found =
        std::ranges::find_if(suites, [&](const auto& suite) { return suite.name == test.suite; });
    if (found == suites.end()) {
      suites.push_back({.name = test.suite, .file = test.file});
      found = std::prev(suites.end());
    }
    auto entry = test.create();
    if (test.create_suite_hooks) {
      auto& state = hooks[test.suite][test.fixture_type];
      if (!state) {
        state = test.create_suite_hooks();
        auto previous = found->finish_fixture;
        found->finish_fixture = [state, previous] {
          finish_suite_hooks(state);
          if (previous) previous();
        };
      }
      entry.set_suite_hooks(state);
    }
    found->cases.push_back(std::move(entry));
  }
}

void check_case_name(std::string_view name) {
  if (name.empty() || name.find_first_of("\r\n\t") != name.npos)
    throw std::logic_error("Case names must be nonempty and contain no tabs or line breaks");
}

std::optional<std::string> condition_skip(const execution_conditions& conditions,
                                          const test_context& context) {
  for (const auto& condition : conditions)
    if (auto reason = condition(context)) return reason;
  return std::nullopt;
}

void copy_context(test_context& target, const test_context& source) {
  target.suite = source.suite;
  target.name = source.name;
  target.parameters = source.parameters;
  target.tags = source.tags;
  target.repetition = source.repetition;
}

void debugger_break() { annotest_platform::debugger_break(); }
inline bool brief_output = false;
inline bool suite_hook_execution = false;

// Temporary files let large writes complete without full pipes or background reader threads.
class std_streams {
 public:
  std_streams() {
    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);
    saved_handles = annotest_platform::save_standard_handles();
    out = annotest_platform::temporary_file();
    err = annotest_platform::temporary_file();
    saved_out = annotest_platform::duplicate(1);
    saved_err = annotest_platform::duplicate(2);
    if (!out || !err || saved_out < 0 || saved_err < 0) {
      restore();
      throw std::runtime_error("Cannot capture test output");
    }
    if (annotest_platform::redirect(annotest_platform::descriptor(out), 1) < 0 ||
        annotest_platform::redirect(annotest_platform::descriptor(err), 2) < 0) {
      restore();
      throw std::runtime_error("Cannot redirect test output");
    }
    annotest_platform::sync_standard_handles();
  }
  std_streams(const std_streams&) = delete;
  std_streams& operator=(const std_streams&) = delete;
  ~std_streams() {
    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);
    if (out) suite_log.testcases.back().sysout = read_file(out);
    if (err) suite_log.testcases.back().syserr = read_file(err);
    restore();
  }

 private:
  static std::string read_file(std::FILE* file) {
    std::rewind(file);
    std::string result;
    char buffer[4096];
    while (auto n = std::fread(buffer, 1, sizeof(buffer), file)) result.append(buffer, n);
    return result;
  }
  void restore() {
    if (saved_out >= 0) {
      annotest_platform::redirect(saved_out, 1);
      annotest_platform::close_descriptor(saved_out);
      saved_out = -1;
    }
    if (saved_err >= 0) {
      annotest_platform::redirect(saved_err, 2);
      annotest_platform::close_descriptor(saved_err);
      saved_err = -1;
    }
    annotest_platform::restore_standard_handles(saved_handles);
    if (out) {
      std::fclose(out);
      out = nullptr;
    }
    if (err) {
      std::fclose(err);
      err = nullptr;
    }
  }
  std::FILE* out = nullptr;
  std::FILE* err = nullptr;
  int saved_out = -1;
  int saved_err = -1;
  annotest_platform::standard_handles saved_handles{};
};

void record_exception(const std::string& phase) {
  try {
    throw;
  } catch (const skip_error& e) {
    suite_log.testcases.back().skip_entries.push_back({e.reason});
  } catch (const assertion_error& e) {
    // Assertion helpers have already recorded their failure; manually thrown errors have not.
    if (!e.recorded()) {
      suite_log.add_failure({failure_types::assertion, e.message(), {.message = e.message()}});
    }
  } catch (const std::exception& e) {
    auto msg = std::format("{}: {}", phase, e.what());
    suite_log.add_failure({failure_types::uncaught_exception, msg, {.message = msg}});
  } catch (...) {
    auto msg = phase + ": unknown exception";
    suite_log.add_failure({failure_types::unknown, msg, {.message = msg}});
  }
}

int execute_test(void* context, std::string_view name, test_callback test, test_callback before,
                 test_callback after, bool extensions) {
  if (!suite_hook_execution) annotest_platform::reset_death_ordinal();
  contract_violation_occurred = false;
  traces.clear();
  suite_log.testcases.emplace_back(
      test_xml_log{.test_class = suite_log.name, .name = std::string(name)});
  auto start = std::chrono::steady_clock::now();
  {
#ifdef _WIN32
    std::optional<std_streams> streams;
    if (!annotest_platform::death_child_active()) streams.emplace();
#else
    std_streams streams;
#endif
    bool ready = true;
    std::vector<std::shared_ptr<test_listener>> entered;
    try {
      current_phase = "extension_setup";
      if (extensions) begin_extensions(entered);
    } catch (...) {
      record_exception(current_phase);
      ready = false;
    }
    if (ready && before) {
      current_phase = "before_each";
      try {
        before(context);
      } catch (...) {
        record_exception(current_phase);
        ready = false;
      }
    }
    if (ready && suite_log.testcases.back().skip_entries.empty()) {
      current_phase = "test";
      try {
        test(context);
      } catch (...) {
        record_exception(current_phase);
      }
    }
    if (after) {
      current_phase = "after_each";
      try {
        after(context);
      } catch (...) {
        record_exception(current_phase);
      }
    }
    if (extensions) {
      end_extensions(entered);
      if (active_context) {
        active_context->resources.clear();
        active_context->directory.reset();
      }
    }
  }
  if (std::exchange(contract_violation_occurred, false)) {
    suite_log.add_failure({failure_types::contract_violation,
                           "Unexpected contract violation",
                           {.message = "Unexpected contract violation"}});
  }
  auto& result = suite_log.testcases.back();
  result.time = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  for (const auto& failure : result.failures) {
    if (failure.type == failure_types::uncaught_exception ||
        failure.type == failure_types::unknown || failure.type == failure_types::contract_violation)
      ++result.errors;
  }
  result.failed = !result.failures.empty();
  suite_log.failed += result.failed;
  suite_log.errors += result.errors;
  if (result.failed)
    result.skip_entries.clear();
  else if (!result.skip_entries.empty())
    ++suite_log.skipped;
  if (!quiet_execution && (!brief_output || result.failed))
    std::println("{}: {}", name,
                 result.failed ? "Failed" : (!result.skip_entries.empty() ? "Skipped" : "Passed"));
  current_phase.clear();
  if (!quiet_execution)
    for (const auto& failure : result.failures) std::println(std::cerr, "{}", failure.cdata);
  return result.failed ? 1 : 0;
}

void begin_extensions(std::vector<std::shared_ptr<test_listener>>& entered) {
  current_phase = "extension_setup";
  for (const auto& listener : listener_snapshot()) {
    entered.push_back(listener);
    listener->before_test();
  }
}
void end_extensions(std::span<const std::shared_ptr<test_listener>> entered) {
  for (auto it = entered.rbegin(); it != entered.rend(); ++it) {
    current_phase = "extension_cleanup";
    try {
      (*it)->after_test();
    } catch (...) {
      record_exception(current_phase);
    }
  }
}
int execute_cleanup(std::string_view name, std::function<void()> cleanup) {
  bool previous = quiet_execution;
  quiet_execution = true;
  struct guard {
    bool previous;
    ~guard() { quiet_execution = previous; }
  } restore{previous};
  return execute_test(
      &cleanup, name, [](void* p) { (*static_cast<std::function<void()>*>(p))(); }, nullptr,
      nullptr, false);
}

void ensure_suite_hooks(const std::shared_ptr<suite_hook_state>& state) {
  if (!state) return;
  ensure_suite_hooks(state->parent);
  if (!state->has_hooks) return;
  if (!state->entered) {
    state->entered = true;
    struct context_guard {
      test_context* previous = active_context;
      bool quiet = quiet_execution;
      bool hook = suite_hook_execution;
      bool contract = contract_violation_occurred;
      std::vector<std::string> saved_traces = std::move(traces);
      std::string phase = std::move(current_phase);
      ~context_guard() {
        active_context = previous;
        quiet_execution = quiet;
        suite_hook_execution = hook;
        contract_violation_occurred = contract || contract_violation_occurred;
        traces = std::move(saved_traces);
        current_phase = std::move(phase);
      }
    } restore;
    active_context = &state->context;
    quiet_execution = true;
    suite_hook_execution = true;
    auto name = suite_log.testcases.back().name;
    try {
      execute_test(
          state.get(), name,
          [](void* pointer) { static_cast<suite_hook_state*>(pointer)->before(); }, nullptr,
          nullptr, false);
    } catch (...) {
      current_phase = "before_all";
      record_exception(current_phase);
      suite_log.testcases.back().failed = true;
    }
    state->preparation = std::move(suite_log.testcases.back());
    suite_log.testcases.pop_back();
  }
  auto& entry = suite_log.testcases.back();
  entry.properties.insert(entry.properties.end(), state->preparation.properties.begin(),
                          state->preparation.properties.end());
  if (!std::exchange(state->claimed, true)) {
    std::cout << state->preparation.sysout;
    std::cerr << state->preparation.syserr;
  }
  if (state->preparation.failed) {
    entry.failures.insert(entry.failures.end(), state->preparation.failures.begin(),
                          state->preparation.failures.end());
    throw assertion_error{"Suite setup failed", true};
  }
  if (!state->preparation.skip_entries.empty())
    skip(state->preparation.skip_entries.front().message);
}

void finish_suite_hooks(const std::shared_ptr<suite_hook_state>& state) {
  if (!state || !state->entered || std::exchange(state->finished, true)) return;
  auto* previous = active_context;
  active_context = &state->context;
  state->after();
  state->context.resources.clear();
  state->context.directory.reset();
  active_context = previous;
}

std::string json_quote(std::string_view text) {
  std::string result = "\"";
  for (unsigned char c : xml_text(text)) {
    if (c == '"' || c == '\\') {
      result += '\\';
      result += c;
    } else if (c < 0x20)
      result += std::format("\\u{:04x}", c);
    else
      result += c;
  }
  return result + "\"";
}
std::string json_strings(std::span<const std::string> values) {
  std::string result = "[";
  for (const auto& value : values) {
    if (result.size() > 1) result += ',';
    result += json_quote(value);
  }
  return result + ']';
}

runner_options parse_options(int argc, char** argv) {
#ifdef _WIN32
  auto utf8 = annotest_platform::utf8_arguments(argc, argv);
  std::vector<char*> arguments;
  if (!utf8.empty()) {
    for (auto& argument : utf8) arguments.push_back(argument.data());
    argv = arguments.data();
  }
#endif
  runner_options options;
  // Environment options are parsed before command-line options.
  std::vector<std::string> environment_arguments;
  std::vector<char*> combined_arguments;
  if (argc > 0 && argv && argv[0]) {
    environment_arguments.emplace_back(argv[0]);
    for (const auto& [variable, option] :
         std::initializer_list<std::pair<const char*, const char*>>{
             {"ANNOTEST_FILTER", "--filter"},
             {"ANNOTEST_EXCLUDE", "--exclude"},
             {"ANNOTEST_TAG", "--tag"},
             {"ANNOTEST_EXCLUDE_TAG", "--exclude-tag"},
             {"ANNOTEST_XML_OUTPUT", "--xml-output"},
             {"ANNOTEST_JSON_OUTPUT", "--json-output"},
             {"ANNOTEST_REPEAT", "--repeat"},
             {"ANNOTEST_SEED", "--seed"},
             {"ANNOTEST_JOBS", "--jobs"},
             {"ANNOTEST_TIMEOUT", "--timeout"},
             {"ANNOTEST_SHARD_COUNT", "--shard-count"},
             {"ANNOTEST_SHARD_INDEX", "--shard-index"}})
      if (const char* value = std::getenv(variable))
        environment_arguments.emplace_back(std::string(option) + "=" + value);
    for (const auto& [variable, option] :
         std::initializer_list<std::pair<const char*, const char*>>{
             {"ANNOTEST_BRIEF", "--brief"},
             {"ANNOTEST_BREAK_ON_FAILURE", "--break-on-failure"},
             {"ANNOTEST_SHUFFLE", "--shuffle"},
             {"ANNOTEST_FAIL_FAST", "--fail-fast"},
             {"ANNOTEST_RUN_DISABLED", "--run-disabled"},
             {"ANNOTEST_ISOLATE", "--isolate"}})
      if (const char* value = std::getenv(variable))
        environment_arguments.emplace_back(std::string(option) + "=" + value);
    for (auto& argument : environment_arguments) combined_arguments.push_back(argument.data());
    for (int i = 1; i < argc; ++i) combined_arguments.push_back(argv[i]);
    argc = combined_arguments.size();
    argv = combined_arguments.data();
  }
  bool show_help = false;
  bool xml_seen = false;
  int environment_count = environment_arguments.size();
  auto error = [&](std::string_view message) {
    std::println(std::cerr, "Error: {}\nUse --help for usage.", message);
    options.should_exit = true;
    options.exit_code = 1;
  };
  auto is_option = [](std::string_view value) { return value.size() > 1 && value.front() == '-'; };
  auto take_tests = [&](int& index, std::string_view attached, bool has_attached) {
    if (has_attached) {
      if (attached.empty()) {
        error("--test-name requires a nonempty test name");
        return;
      }
      options.test_names.emplace_back(attached);
    }
    auto previous_size = options.test_names.size();
    while (index + 1 < argc && argv[index + 1] && !is_option(argv[index + 1])) {
      std::string_view name = argv[++index];
      if (name.empty()) {
        error("--test-name requires a nonempty test name");
        return;
      }
      options.test_names.emplace_back(name);
    }
    if (!has_attached && options.test_names.size() == previous_size) {
      error("--test-name requires at least one test name");
    }
  };
  auto take_xml = [&](int& index, std::string_view attached, bool has_attached) {
    if (xml_seen) {
      error("--xml-output may only be specified once");
      return;
    }
    xml_seen = true;
    if (!has_attached) {
      if (index + 1 >= argc || !argv[index + 1] || is_option(argv[index + 1])) {
        error("--xml-output requires a file path");
        return;
      }
      attached = argv[++index];
    }
    if (attached.empty()) {
      error("--xml-output requires a nonempty file path");
      return;
    }
    options.xml_output = attached;
  };

  for (int i = 1; i < argc && !options.should_exit; ++i) {
    if (!argv || !argv[i]) {
      error("Invalid command-line argument");
      break;
    }
    if (i == environment_count) xml_seen = false;
    std::string_view arg = argv[i];
#ifdef _WIN32
    if (arg.starts_with("--annotest-death-name=")) {
      auto value = arg.substr(std::string_view("--annotest-death-name=").size());
      std::string name;
      bool valid = !value.empty() && value.size() % 2 == 0;
      for (std::size_t offset = 0; valid && offset < value.size(); offset += 2) {
        unsigned byte = 0;
        auto [end, ec] =
            std::from_chars(value.data() + offset, value.data() + offset + 2, byte, 16);
        valid = ec == std::errc{} && end == value.data() + offset + 2 && byte != 0;
        name += static_cast<char>(byte);
      }
      if (valid)
        options.test_names.emplace_back(name);
      else
        error("Invalid death-test child name");
      continue;
    }
    if (arg.starts_with("--annotest-death-child=")) {
      auto value = arg.substr(std::string_view("--annotest-death-child=").size());
      auto separator = value.find(':');
      std::uintptr_t handle = 0;
      auto first = value.substr(0, separator);
      auto second =
          separator == std::string_view::npos ? std::string_view{} : value.substr(separator + 1);
      auto [end2, ec2] = std::from_chars(second.data(), second.data() + second.size(), handle);
      if (ec2 != std::errc{} || end2 != second.data() + second.size() || handle == 0 ||
          !annotest_platform::configure_death_child(first, handle)) {
        error("Invalid death-test child arguments");
      }
      continue;
    }
#endif
    if (arg == "--") {
      if (i + 1 < argc) error("Positional arguments are not supported");
      break;
    }
    if (arg.starts_with("--")) {
      auto equal = arg.find('=');
      auto name = arg.substr(0, equal);
      bool has_value = equal != std::string_view::npos;
      auto value = has_value ? arg.substr(equal + 1) : std::string_view{};
      auto take_value = [&]() -> std::string_view {
        if (!has_value) {
          if (i + 1 >= argc || !argv[i + 1] || is_option(argv[i + 1])) {
            error(std::format("{} requires a value", name));
            return {};
          }
          value = argv[++i];
        }
        if (value.empty()) error(std::format("{} requires a nonempty value", name));
        return value;
      };
      auto integer = [&](unsigned& target, bool zero = false) {
        auto text = take_value();
        unsigned parsed = 0;
        auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), parsed);
        if (ec != std::errc{} || end != text.data() + text.size() || (!zero && !parsed))
          error(
              std::format("{} requires {}integer", name, zero ? "a nonnegative " : "a positive "));
        else
          target = parsed;
      };
      if (name == "--json-output") {
        options.json_output = take_value();
      } else if (name == "--filter" || name == "--exclude" || name == "--tag" ||
                 name == "--exclude-tag") {
        auto text = take_value();
        auto& destination = name == "--filter"    ? options.includes
                            : name == "--exclude" ? options.excludes
                            : name == "--tag"     ? options.include_tags
                                                  : options.exclude_tags;
        destination.emplace_back(text);
      } else if (name == "--repeat")
        integer(options.repeat);
      else if (name == "--seed") {
        integer(options.seed, true);
        options.seed_given = true;
        options.shuffle = true;
      } else if (name == "--jobs")
        integer(options.jobs);
      else if (name == "--shard-count")
        integer(options.shard_count);
      else if (name == "--shard-index")
        integer(options.shard_index, true);
      else if (name == "--timeout") {
        auto text = take_value();
        double parsed = 0;
        auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), parsed);
        if (ec != std::errc{} || end != text.data() + text.size() || !std::isfinite(parsed) ||
            parsed <= 0)
          error("--timeout requires positive finite seconds");
        else
          options.timeout_seconds = parsed;
      } else if (name == "--worker-repetition") {
        integer(options.worker_repetition, true);
      } else if (name == "--worker-result") {
        options.worker_result = take_value();
        options.worker = true;
      } else if (name == "--run-disabled" || name == "--shuffle" || name == "--fail-fast" ||
                 name == "--isolate" || name == "--brief" || name == "--break-on-failure") {
        bool enabled = true;
        if (has_value) {
          if (value != "0" && value != "1") error(std::format("{} accepts only 0 or 1", name));
          enabled = value == "1";
        }
        if (name == "--run-disabled")
          options.run_disabled = enabled;
        else if (name == "--shuffle")
          options.shuffle = enabled;
        else if (name == "--brief")
          options.brief = enabled;
        else if (name == "--break-on-failure")
          options.break_on_failure = enabled;
        else if (name == "--fail-fast")
          options.fail_fast = enabled;
        else
          options.isolate = enabled;
      } else if (name == "--list" || name == "--list-json" || name == "--help") {
        if (has_value) {
          error(std::format("{} does not accept a value", name));
        } else if (name == "--list" || name == "--list-json") {
          options.list_tests = true;
          options.list_json |= name == "--list-json";
        } else {
          show_help = true;
        }
      } else if (name == "--test-name") {
        take_tests(i, value, has_value);
      } else if (name == "--xml-output") {
        take_xml(i, value, has_value);
      } else {
        error(std::format("Unknown option: {}", name));
      }
    } else if (is_option(arg)) {
      for (std::size_t position = 1; position < arg.size() && !options.should_exit; ++position) {
        char option = arg[position];
        if (option == 'l') {
          options.list_tests = true;
        } else if (option == 'h') {
          show_help = true;
        } else if (option == 't') {
          auto value = arg.substr(position + 1);
          bool has_value = !value.empty();
          if (value.starts_with('=')) value.remove_prefix(1);
          take_tests(i, value, has_value);
          break;
        } else {
          error(std::format("Unknown option: -{}", option));
        }
      }
    } else {
      error(std::format("Unexpected positional argument: {}", arg));
    }
  }

  if (options.shard_index >= options.shard_count)
    error("--shard-index must be less than --shard-count");
  if (options.jobs > 256 || options.repeat > 1000000)
    error("Execution count exceeds supported limit");
  if (show_help && !options.should_exit) {
    auto program = argc > 0 && argv && argv[0] ? argv[0] : "tests";
    std::println(
        "AnnoTest Based Tests\n"
        "Usage: {} [OPTIONS]\n\n"
        "Options:\n"
        "  -h, --help                  Show this help and exit\n"
        "  -l, --list                  List all available tests\n"
        "      --list-json            List test metadata as JSON\n"
        "  -t, --test-name NAME...     Run fully qualified test names (repeatable)\n"
        "      --xml-output FILE      Write an XML report to this file\n"
        "      --filter GLOB          Include matching fully qualified names (repeatable)\n"
        "      --exclude GLOB         Exclude matching names (repeatable)\n"
        "      --tag TAG              Include tagged tests (repeatable)\n"
        "      --exclude-tag TAG      Exclude tagged tests (repeatable)\n"
        "      --run-disabled         Execute disabled tests\n"
        "      --repeat N             Repeat selected cases\n"
        "      --shuffle [--seed N]   Shuffle with a reproducible seed\n"
        "      --json-output FILE    Write JSON results\n"
        "      --brief               Print failures only\n"
        "      --break-on-failure    Trigger a debugger breakpoint\n"
        "      --fail-fast            Stop scheduling after a failure\n"
        "      --shard-count N --shard-index I  Select deterministic shard (zero based)\n"
        "      --timeout SECONDS      Hard deadline per case using a subprocess\n"
        "      --isolate              Execute every case in a subprocess\n"
        "      --jobs N               Execute up to N subprocesses concurrently",
        program);
    options.should_exit = true;
  }
  return options;
}

std::chrono::steady_clock::time_point begin_suite(std::string_view suite_name, const char* file,
                                                  bool filtered) {
  suite_log.reset();
  auto suite_start = std::chrono::steady_clock::now();
  suite_log.name = std::string(suite_name);
  suite_log.file = file;
  suite_log.total_tests = 0;
  suite_log.timestamp = std::format(
      "{:%FT%TZ}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));

  if (brief_output) return suite_start;
  if (!filtered) {
    std::println("Running tests in suite {}", suite_name);
  } else {
    std::println("Running specified tests in suite {}", suite_name);
  }

  return suite_start;
}

}  // namespace annotest

namespace annotest {
namespace {
bool glob_match(std::string_view pattern, std::string_view value) {
  std::size_t p = 0, v = 0, star = pattern.npos, restart = 0;
  while (v < value.size()) {
    if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == value[v])) {
      ++p;
      ++v;
    } else if (p < pattern.size() && pattern[p] == '*') {
      star = p++;
      restart = v;
    } else if (star != pattern.npos) {
      p = star + 1;
      v = ++restart;
    } else
      return false;
  }
  while (p < pattern.size() && pattern[p] == '*') ++p;
  return p == pattern.size();
}
std::uint64_t shard_hash(std::string_view value) {
  std::uint64_t hash = 14695981039346656037ULL;
  for (unsigned char byte : value) {
    hash ^= byte;
    hash *= 1099511628211ULL;
  }
  return hash;
}
std::string read_bytes(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(input), {}};
}
void put_string(std::ostream& stream, const std::string& value) {
  std::uint64_t size = value.size();
  stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
  stream.write(value.data(), value.size());
}
std::string get_string(std::istream& stream) {
  std::uint64_t size = 0;
  stream.read(reinterpret_cast<char*>(&size), sizeof(size));
  if (!stream || size > 64 * 1024 * 1024) throw std::runtime_error("Invalid worker result");
  std::string value(size, '\0');
  stream.read(value.data(), value.size());
  if (!stream) throw std::runtime_error("Incomplete worker result");
  return value;
}
void write_worker(const std::string& path, const test_xml_log& test) {
  std::ofstream stream(std::filesystem::path(std::u8string(path.begin(), path.end())),
                       std::ios::binary);
  put_string(stream, "AnnoTest worker 2");
  put_string(stream, std::to_string(test.time));
  put_string(stream, test.sysout);
  put_string(stream, test.syserr);
  put_string(stream, test.skip_entries.empty() ? "0" : "1");
  put_string(stream, test.skip_entries.empty() ? "" : test.skip_entries.front().message);
  put_string(stream, std::to_string(test.failures.size()));
  for (const auto& failure : test.failures) {
    put_string(stream, std::to_string(static_cast<int>(failure.type)));
    put_string(stream, failure.message);
    put_string(stream, failure.phase);
    put_string(stream, failure.cdata.message);
    put_string(stream, failure.cdata.location ? "1" : "0");
    if (failure.cdata.location) {
      put_string(stream, failure.cdata.location->first);
      put_string(stream, std::to_string(failure.cdata.location->second));
    }
    bool has_trace =
        failure.cdata.decoded_stacktrace.has_value() || failure.cdata.stacktrace.has_value();
    put_string(stream, has_trace ? "1" : "0");
    if (has_trace)
      put_string(stream, failure.cdata.decoded_stacktrace
                             ? *failure.cdata.decoded_stacktrace
                             : std::format("{}", *failure.cdata.stacktrace));
  }
  put_string(stream, std::to_string(test.properties.size()));
  for (const auto& property : test.properties) {
    put_string(stream, property.name);
    put_string(stream, property.value);
  }
  stream.flush();
  if (!stream) throw std::runtime_error("Cannot write worker result");
}
test_xml_log read_worker(const std::filesystem::path& path, std::string suite, std::string name) {
  std::ifstream stream(path, std::ios::binary);
  if (get_string(stream) != "AnnoTest worker 2")
    throw std::runtime_error("Invalid worker protocol");
  test_xml_log test{.test_class = std::move(suite), .name = std::move(name)};
  test.time = std::stod(get_string(stream));
  test.sysout = get_string(stream);
  test.syserr = get_string(stream);
  bool is_skipped = get_string(stream) == "1";
  auto skipped = get_string(stream);
  if (is_skipped) test.skip_entries.push_back({skipped});
  auto count = std::stoul(get_string(stream));
  if (count > 100000) throw std::runtime_error("Invalid worker failure count");
  for (unsigned i = 0; i < count; ++i) {
    auto kind = std::stoi(get_string(stream));
    if (kind < 0 || kind > static_cast<int>(failure_types::unknown))
      throw std::runtime_error("Invalid worker failure type");
    auto message = get_string(stream), phase = get_string(stream), detail = get_string(stream);
    failure_cdata data{.message = detail, .stacktrace = std::nullopt};
    if (get_string(stream) == "1") {
      auto file = get_string(stream);
      auto line = std::stoull(get_string(stream));
      data.location = std::pair{std::move(file), static_cast<std::size_t>(line)};
    }
    if (get_string(stream) == "1") data.decoded_stacktrace = get_string(stream);
    test.failures.push_back({static_cast<failure_types>(kind), message, std::move(data), phase});
  }
  count = std::stoul(get_string(stream));
  if (count > 100000) throw std::runtime_error("Invalid worker property count");
  for (unsigned i = 0; i < count; ++i) {
    auto key = get_string(stream), value = get_string(stream);
    test.properties.push_back({std::move(key), std::move(value)});
  }
  test.failed = !test.failures.empty();
  for (const auto& failure : test.failures)
    if (failure.type == failure_types::uncaught_exception ||
        failure.type == failure_types::unknown || failure.type == failure_types::contract_violation)
      ++test.errors;
  return test;
}
void write_report(const std::string& path, std::string_view contents) {
  std::ofstream output(std::filesystem::path(std::u8string(path.begin(), path.end())));
  output << contents;
  output.flush();
  if (!output) throw std::runtime_error("Cannot write report file");
}
}  // namespace

int run_plan(std::vector<execution_suite> suites, const runner_options& requested_options, int argc,
             char** argv) {
  auto options = requested_options;
  if (options.worker) {
    options.repeat = 1;
    options.shard_count = 1;
    options.shard_index = 0;
    options.xml_output.clear();
    options.json_output.clear();
  }
  // Capturing stdout and the active logger are process-wide. Reject recursive runs explicitly.
  static std::atomic_flag running{};
  if (running.test_and_set()) throw std::logic_error("Recursive test runners are unsupported");
  struct guard {
    std::atomic_flag& value;
    ~guard() {
      active_context = nullptr;
      current_phase.clear();
      listener_selection_active = false;
      selected_listeners.clear();
      quiet_execution = false;
      brief_output = false;
      break_on_failure = false;
      value.clear();
    }
  } cleanup{running};
  observer_failures.clear();
  brief_output = options.brief;
  break_on_failure = options.break_on_failure && !options.worker;
  for (auto& suite : suites) {
    select_listeners(suite.name);
    suite_log.reset();
    suite_log.name = suite.name;
    std::vector<execution_case> expanded;
    bool previous = quiet_execution;
    quiet_execution = true;
    for (auto& test : suite.cases) {
      if (test.expand && (options.list_tests || !test.disabled || options.run_disabled)) {
        auto children = test.expand();
        for (auto& child : children) expanded.push_back(std::move(child));
      } else
        expanded.push_back(std::move(test));
    }
    quiet_execution = previous;
    suite.cases = std::move(expanded);
  }
  std::unordered_set<std::string> names, bases, suite_names;
  for (const auto& suite : suites) {
    if (!suite_names.insert(suite.name).second)
      throw std::logic_error("Duplicate suite name: " + suite.name);
    for (const auto& test : suite.cases) {
      auto name = suite.name + "." + test.name;
      if (!names.insert(name).second) throw std::logic_error("Duplicate full test name: " + name);
      bases.insert(suite.name + "." + test.base_name);
      select_listeners(suite.name);
      notify_listeners("on_discovery", [&](test_listener& listener) {
        listener.on_discovery(suite.name, test.name);
        listener.on_test_discovered({suite.name, test.name, test.file, test.parameters, test.line,
                                     test.tags, test.disabled});
      });
    }
  }
  if (options.list_tests) {
    if (options.list_json) {
      std::string json = "{\"schema_version\":1,\"suites\":[";
      bool first_suite = true;
      for (const auto& suite : suites) {
        if (!std::exchange(first_suite, false)) json += ',';
        json += "{\"name\":" + json_quote(suite.name) + ",\"file\":" + json_quote(suite.file) +
                ",\"tests\":[";
        bool first_test = true;
        for (const auto& test : suite.cases) {
          if (!std::exchange(first_test, false)) json += ',';
          json += "{\"name\":" + json_quote(test.name) +
                  ",\"full_name\":" + json_quote(suite.name + "." + test.name) +
                  ",\"base_name\":" + json_quote(suite.name + "." + test.base_name) +
                  ",\"file\":" + json_quote(test.file) + ",\"line\":" + std::to_string(test.line) +
                  ",\"parameters\":" + json_quote(test.parameters) +
                  ",\"tags\":" + json_strings(test.tags) +
                  ",\"disabled\":" + (test.disabled ? "true" : "false") +
                  ",\"skip_reason\":" + json_quote(test.skip_reason) + "}";
        }
        json += "]}";
      }
      std::println("{}", json + "]}");
    } else {
      for (const auto& suite : suites) {
        std::println("{}.", suite.name);
        for (const auto& test : suite.cases) std::println("\t{}", test.name);
      }
    }
    bool failed = false;
    for (std::size_t index = suites.size(); index-- > 0;) {
      auto& suite = suites[index];
      select_listeners(suite.name);
      suite_log.reset();
      suite_log.name = suite.name;
      std::unordered_set<std::string> finished;
      for (auto& test : suite.cases) {
        failed |= test.discovery_failed;
        if (test.finish_group && finished.insert(test.group_name).second)
          execute_cleanup(test.group_name, test.finish_group);
      }
      if (suite.finish_fixture) execute_cleanup("_fixture_teardown", suite.finish_fixture);
      for (const auto& entry : suite_log.testcases) failed |= entry.failed;
    }
    failed |= !observer_failures.empty();
    if (failed) std::println(std::cerr, "Cannot discover dynamic test factory");
    return failed ? 1 : 0;
  }
  bool unknown_names = false;
  for (const auto& requested : options.test_names) {
    if (!names.contains(requested) && !bases.contains(requested)) {
      // Preserve a useful diagnostic for malformed parameter selectors.
      auto dot = requested.rfind('.');
      if (std::ranges::any_of(bases,
                              [&](const auto& base) { return requested.starts_with(base + "."); }))
        std::println(std::cerr, "Invalid parameterization: {}", requested);
      else
        std::println(std::cerr, "Warning: The following test names were not found: {}", requested);
      unknown_names = true;
    }
  }
  auto any_match = [](const auto& patterns, std::string_view name) {
    return std::ranges::any_of(patterns,
                               [&](const auto& pattern) { return glob_match(pattern, name); });
  };
  auto has_tag = [](const auto& requested, const auto& actual) {
    return std::ranges::any_of(
        requested, [&](const auto& tag) { return std::ranges::find(actual, tag) != actual.end(); });
  };
  unsigned seed = options.seed;
  if (options.shuffle && !options.seed_given) seed = std::random_device{}();
  if (options.shuffle) std::println("Shuffle seed: {}", seed);
  std::mt19937 random(seed);
  int status = unknown_names ? 1 : 0;
  std::vector<test_result> results;
  std::vector<suite_xml_log> reports;
  bool stopped = false;
  for (auto& suite : suites) {
    auto started = begin_suite(suite.name, suite.file.c_str(), !options.test_names.empty());
    select_listeners(suite.name);
    notify_listeners("on_suite_start",
                     [&](test_listener& listener) { listener.on_suite_start(suite.name); });
    std::vector<execution_case*> selected;
    for (auto& test : suite.cases) {
      const auto name = suite.name + "." + test.name;
      const auto base = suite.name + "." + test.base_name;
      if (!options.test_names.empty() &&
          std::ranges::none_of(options.test_names, [&](const auto& filter) {
            return filter == name || (filter == base && !names.contains(filter));
          }))
        continue;
      if (!options.includes.empty() && !any_match(options.includes, name)) continue;
      if (any_match(options.excludes, name)) continue;
      if (!options.include_tags.empty() && !has_tag(options.include_tags, test.tags)) continue;
      if (has_tag(options.exclude_tags, test.tags)) continue;
      if (shard_hash(name) % options.shard_count != options.shard_index) continue;
      selected.push_back(&test);
    }
    if (!options.test_names.empty()) {
      auto rank = [&](const execution_case* test) {
        auto name = suite.name + "." + test->name;
        auto base = suite.name + "." + test->base_name;
        for (std::size_t i = 0; i < options.test_names.size(); ++i)
          if (options.test_names[i] == name || options.test_names[i] == base) return i;
        return options.test_names.size();
      };
      std::stable_sort(selected.begin(), selected.end(), [&](auto* a, auto* b) {
        if (a->base_name != b->base_name) {
          return std::ranges::find_if(suite.cases, [&](const auto& v) { return &v == a; }) <
                 std::ranges::find_if(suite.cases, [&](const auto& v) { return &v == b; });
        }
        return rank(a) < rank(b);
      });
    }
    struct invocation {
      execution_case* test;
      unsigned repetition;
    };
    std::vector<invocation> invocations;
    for (auto* test : selected) {
      if (test->repetitions > 1000000 / options.repeat)
        throw std::logic_error("Combined test repetitions must not exceed 1000000");
      auto count = options.worker ? 1u : test->repetitions;
      for (unsigned repetition = 0; repetition < count; ++repetition)
        invocations.push_back({test, repetition});
    }
    auto accept = [&](test_xml_log entry, execution_case& test, unsigned repetition,
                      test_context* context = nullptr) {
      auto* previous_context = active_context;
      active_context = context;
      entry.file = test.file;
      entry.line = test.line;
      test_result result{
          suite.name,
          test.name,
          entry.failed ? test_status::failed
                       : (!entry.skip_entries.empty() ? test_status::skipped : test_status::passed),
          entry.time,
          entry.skip_entries.empty() ? "" : entry.skip_entries.front().message,
          entry.failures.size()};
      if (entry.failed) {
        if (break_on_failure && (options.isolate || options.jobs > 1 || test.timeout_seconds > 0 ||
                                 options.timeout_seconds > 0))
          debugger_break();
        status = 1;
        if (options.fail_fast) stopped = true;
      }
      std::erase_if(entry.properties, [](const auto& property) {
        return property.name == "repetition" || property.name == "tags";
      });
      entry.properties.push_back({"repetition", std::to_string(repetition + 1)});
      if (!test.tags.empty()) {
        std::string labels;
        for (const auto& label : test.tags) {
          if (!labels.empty()) labels += ",";
          labels += label;
        }
        entry.properties.push_back({"tags", labels});
      }
      for (const auto& failure : entry.failures)
        result.details.push_back(
            {failure.type, failure.message, failure.phase, failure.cdata.location});
      for (const auto& property : entry.properties)
        result.properties.emplace_back(property.name, property.value);
      result.standard_output = entry.sysout;
      result.standard_error = entry.syserr;
      result.file = test.file;
      result.line = test.line;
      result.parameters = test.parameters;
      result.tags = test.tags;
      results.push_back(result);
      notify_listeners("on_test_end",
                       [&](test_listener& listener) { listener.on_test_end(result); });
      suite_log.testcases.push_back(std::move(entry));
      active_context = previous_context;
    };
    auto isolated = [&](execution_case& test, unsigned repetition) {
      temporary_directory scratch;
      auto dir = scratch.path();
      auto bytes = [](const std::filesystem::path& path) {
        auto u = path.u8string();
        return std::string(u.begin(), u.end());
      };
      auto result_path = dir / "result.bin";
      std::vector<std::string> args{argc > 0 && argv && argv[0] ? argv[0] : "",
                                    "--test-name=" + suite.name + "." + test.name,
                                    "--worker-result=" + bytes(result_path), "--run-disabled",
                                    "--worker-repetition=" + std::to_string(repetition)};
      auto deadline = options.timeout_seconds > 0 ? options.timeout_seconds : test.timeout_seconds;
      auto start = std::chrono::steady_clock::now();
      auto child = annotest_platform::run_process(args, bytes(dir / "stdout"),
                                                  bytes(dir / "stderr"), deadline);
      test_xml_log entry{.test_class = suite.name, .name = test.name};
      try {
        if (child.timed_out || !child.detail.empty() || child.exit_code < 0 || child.exit_code > 1)
          throw std::runtime_error(child.detail.empty() ? "Test worker crashed" : child.detail);
        entry = read_worker(result_path, suite.name, test.name);
        if (child.exit_code != static_cast<int>(entry.failed))
          throw std::runtime_error("Worker exit status disagrees with result");
      } catch (const std::exception& error) {
        // A rejected worker result is untrusted, including any partially decoded failures.
        entry = test_xml_log{.test_class = suite.name, .name = test.name};
        entry.failed = true;
        entry.errors = 1;
        entry.time =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        entry.sysout = read_bytes(dir / "stdout");
        entry.syserr = read_bytes(dir / "stderr");
        entry.failures.push_back({failure_types::uncaught_exception,
                                  error.what(),
                                  {.message = error.what(), .stacktrace = std::nullopt},
                                  child.timed_out ? "timeout" : "process"});
      }
      return entry;
    };
    for (unsigned repetition = 0; repetition < options.repeat && !stopped; ++repetition) {
      if (options.shuffle) std::shuffle(invocations.begin(), invocations.end(), random);
      for (std::size_t offset = 0; offset < invocations.size() && !stopped;) {
        struct pending {
          execution_case* test;
          std::future<test_xml_log> result;
          std::optional<test_xml_log> immediate;
          std::shared_ptr<test_context> context;
          bool isolated = false;
          unsigned repetition = 0;
        };
        std::vector<pending> workers;
        for (unsigned slot = 0; slot < options.jobs && offset < invocations.size() && !stopped;
             ++slot, ++offset) {
          auto& test = *invocations[offset].test;
          auto case_repetition =
              options.worker ? options.worker_repetition
                             : repetition * test.repetitions + invocations[offset].repetition;
          auto context = std::make_shared<test_context>();
          context->suite = suite.name;
          context->name = test.name;
          context->parameters = test.parameters;
          context->tags = test.tags;
          context->repetition = case_repetition;
          active_context = context.get();
          notify_listeners("on_test_start", [&](test_listener& listener) {
            listener.on_test_start(suite.name, test.name);
          });
          auto skipped = test.skip_reason;
          if (test.disabled && !options.run_disabled) skipped = "Disabled test";
          std::optional<test_xml_log> preparation;
          bool isolate =
              !options.worker && (options.isolate || options.jobs > 1 ||
                                  options.timeout_seconds > 0 || test.timeout_seconds > 0);
          if (skipped.empty() && !isolate && !test.conditions.empty()) {
            bool previous = quiet_execution;
            quiet_execution = true;
            execute_test(
                &test, test.name,
                [](void* pointer) {
                  current_phase = "condition";
                  auto& test = *static_cast<execution_case*>(pointer);
                  if (auto reason = condition_skip(test.conditions, current_test())) skip(*reason);
                },
                nullptr, nullptr, false);
            quiet_execution = previous;
            preparation = std::move(suite_log.testcases.back());
            suite_log.testcases.pop_back();
          }
          if (!skipped.empty()) {
            test_xml_log entry{.test_class = suite.name, .name = test.name};
            entry.skip_entries.push_back({skipped});
            workers.push_back({&test, {}, std::move(entry), context, false, case_repetition});
          } else if (preparation && (preparation->failed || !preparation->skip_entries.empty())) {
            workers.push_back({&test, {}, std::move(preparation), context, false, case_repetition});
          } else if (isolate) {
            workers.push_back({&test,
                               std::async(std::launch::async,
                                          [&, target = &test, case_repetition] {
                                            return isolated(*target, case_repetition);
                                          }),
                               std::nullopt, context, true, case_repetition});
          } else {
            active_context = context.get();
            auto size = suite_log.testcases.size();
            try {
              if (!test.run) throw std::logic_error("Test has no runnable callback");
              test.run();
            } catch (...) {
              if (suite_log.testcases.size() == size)
                suite_log.testcases.push_back({.test_class = suite.name, .name = test.name});
              current_phase = "runner";
              record_exception(current_phase);
              std::println(std::cerr, "{}", suite_log.testcases.back().failures.back().message);
              suite_log.testcases.back().failed = true;
              suite_log.testcases.back().errors = 1;
            }
            active_context = nullptr;
            auto entry = std::move(suite_log.testcases.back());
            suite_log.testcases.pop_back();
            if (preparation) {
              entry.sysout = preparation->sysout + entry.sysout;
              entry.syserr = preparation->syserr + entry.syserr;
            }
            if (!test.parameters.empty() &&
                std::ranges::none_of(entry.properties, [](const auto& property) {
                  return property.name == "parameters";
                }))
              entry.properties.push_back({"parameters", test.parameters});

            if (entry.failed && options.fail_fast) stopped = true;
            workers.push_back({&test, {}, std::move(entry), context, false, case_repetition});
          }
          active_context = nullptr;
        }
        for (auto& worker : workers) {
          auto entry = worker.immediate ? std::move(*worker.immediate) : worker.result.get();
          active_context = worker.context.get();
          if (worker.isolated)
            for (const auto& failure : entry.failures)
              notify_listeners("on_assertion", [&](test_listener& listener) {
                listener.on_assertion({failure.message, failure.phase, failure.cdata.location});
              });
          accept(std::move(entry), *worker.test, worker.repetition, worker.context.get());
          active_context = nullptr;
        }
      }
    }
    std::unordered_set<std::string> finished_groups;
    for (auto& test : suite.cases)
      if (test.finish_group && finished_groups.insert(test.group_name).second) {
        test_context context{.suite = suite.name, .name = test.group_name + "._teardown"};
        active_context = &context;
        execute_cleanup(context.name, test.finish_group);
        active_context = nullptr;
        auto entry = std::move(suite_log.testcases.back());
        suite_log.testcases.pop_back();
        if (entry.failed || !entry.sysout.empty() || !entry.syserr.empty()) {
          if (entry.failed) status = 1;
          // Worker teardown belongs to its selected case, preserving the single-case protocol.
          if (options.worker && !suite_log.testcases.empty()) {
            auto& target = suite_log.testcases.back();
            target.failures.insert(target.failures.end(), entry.failures.begin(),
                                   entry.failures.end());
            target.failed |= entry.failed;
            target.errors += entry.errors;
            target.sysout += entry.sysout;
            target.syserr += entry.syserr;
          } else if (entry.failed) {
            execution_case cleanup_case{.name = entry.name};
            notify_listeners("on_test_start", [&](test_listener& listener) {
              listener.on_test_start(suite.name, entry.name);
            });
            accept(std::move(entry), cleanup_case, 0);
          } else if (!suite_log.testcases.empty()) {
            suite_log.testcases.back().sysout += entry.sysout;
            suite_log.testcases.back().syserr += entry.syserr;
          }
        }
      }
    suite_log.total_tests = suite_log.testcases.size();
    suite_log.failed = suite_log.skipped = suite_log.errors = 0;
    for (const auto& test : suite_log.testcases) {
      suite_log.failed += test.failed;
      suite_log.skipped += !test.skip_entries.empty();
      suite_log.errors += test.errors;
    }
    suite_log.time =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
    reports.push_back(suite_log);
    notify_listeners("on_suite_end",
                     [&](test_listener& listener) { listener.on_suite_end(suite.name); });
  }
  for (std::size_t i = suites.size(); i-- > 0;) {
    auto& suite = suites[i];
    select_listeners(suite.name);
    suite_log = reports[i];
    test_context context{.suite = suite.name, .name = "_fixture_teardown"};
    active_context = &context;
    if (suite.finish_fixture) {
      execute_cleanup(context.name, suite.finish_fixture);
      auto entry = std::move(suite_log.testcases.back());
      suite_log.testcases.pop_back();
      // A parent selected only through a nested suite has no case of its own.
      // Preserve its successful cleanup output on the last participating descendant.
      test_xml_log* target = suite_log.testcases.empty() ? nullptr : &suite_log.testcases.back();
      if (!target) {
        for (std::size_t child = i + 1; child < reports.size(); ++child)
          if (reports[child].name.starts_with(suite.name + ".") &&
              !reports[child].testcases.empty())
            target = &reports[child].testcases.back();
      }
      if (options.worker && target) {
        target->failures.insert(target->failures.end(), entry.failures.begin(),
                                entry.failures.end());
        target->failed |= entry.failed;
        target->errors += entry.errors;
        if (target->failed) target->skip_entries.clear();
        target->sysout += entry.sysout;
        target->syserr += entry.syserr;
        status |= entry.failed;
      } else if (entry.failed) {
        status = 1;
        test_result result{suite.name, entry.name, test_status::failed,
                           entry.time, "",         entry.failures.size()};
        for (const auto& failure : entry.failures)
          result.details.push_back(
              {failure.type, failure.message, failure.phase, failure.cdata.location});
        result.standard_output = entry.sysout;
        result.standard_error = entry.syserr;
        notify_listeners("on_test_start", [&](test_listener& listener) {
          listener.on_test_start(suite.name, entry.name);
        });
        results.push_back(result);
        notify_listeners("on_test_end",
                         [&](test_listener& listener) { listener.on_test_end(result); });
        suite_log.testcases.push_back(std::move(entry));
      } else if (target) {
        target->sysout += entry.sysout;
        target->syserr += entry.syserr;
      }
    }
    active_context = nullptr;
    reports[i] = suite_log;
  }
  // Index repeated results in execution order. Suite teardown may append output
  // after on_test_end, so refresh the snapshots passed to the final reporters.
  std::unordered_map<std::string, std::vector<std::size_t>> result_indices;
  std::unordered_map<std::string, const test_result*> result_metadata;
  for (std::size_t index = 0; index < results.size(); ++index) {
    auto name = results[index].suite + "." + results[index].name;
    result_indices[name].push_back(index);
    result_metadata.try_emplace(name, &results[index]);
  }
  std::unordered_map<std::string, std::size_t> consumed;
  for (const auto& report : reports)
    for (const auto& entry : report.testcases) {
      auto name = report.name + "." + entry.name;
      auto found = result_indices.find(name);
      auto& next = consumed[name];
      if (found != result_indices.end() && next < found->second.size()) {
        auto& result = results[found->second[next++]];
        result.standard_output = entry.sysout;
        result.standard_error = entry.syserr;
      }
    }
  // Each registration receives final results once, regardless of its suite scope.
  for (const auto& registration : listeners) {
    selected_listeners = {registration};
    const auto& scope = listener_scopes[registration.get()];
    std::vector<test_result> scoped_results;
    for (const auto& result : results)
      if (scope.empty() || result.suite == scope || result.suite.starts_with(scope + "."))
        scoped_results.push_back(result);
    notify_listeners("on_run_end",
                     [&](test_listener& listener) { listener.on_run_end(scoped_results); });
  }
  for (const auto& report : reports) {
    select_listeners(report.name);
    suite_result summary{.name = report.name,
                         .tests = static_cast<unsigned>(report.testcases.size()),
                         .seconds = report.time};
    for (const auto& entry : report.testcases) {
      summary.failed += entry.failed;
      summary.skipped += !entry.skip_entries.empty();
      summary.errors += entry.errors;
    }
    notify_listeners("on_suite_result",
                     [&](test_listener& listener) { listener.on_suite_result(summary); });
  }
  for (const auto& error : observer_failures) {
    auto found = std::ranges::find_if(
        reports, [&](const auto& report) { return report.name == error.suite; });
    if (found == reports.end()) found = reports.begin();
    if (found == reports.end()) continue;
    test_xml_log entry{
        .test_class = found->name, .name = "_listener." + error.phase, .failed = true, .errors = 1};
    entry.failures.push_back({failure_types::uncaught_exception,
                              error.message,
                              {.message = error.message, .stacktrace = std::nullopt},
                              error.phase});
    found->testcases.push_back(std::move(entry));
    status = 1;
  }
  for (auto& report : reports) {
    report.total_tests = report.testcases.size();
    report.failed = report.errors = report.skipped = 0;
    for (const auto& entry : report.testcases) {
      report.failed += entry.failed;
      report.errors += entry.errors;
      report.skipped += !entry.skip_entries.empty();
    }
  }
  if (options.worker) {
    for (const auto& report : reports)
      if (!report.testcases.empty()) {
        auto entry = report.testcases.front();
        for (std::size_t i = 1; i < report.testcases.size(); ++i) {
          const auto& extra = report.testcases[i];
          entry.failures.insert(entry.failures.end(), extra.failures.begin(), extra.failures.end());
          entry.failed |= extra.failed;
        }
        if (entry.failed) entry.skip_entries.clear();
        write_worker(options.worker_result, entry);
        break;
      }
  }
  if (!options.json_output.empty()) {
    try {
      auto quote = json_quote;
      unsigned total = 0, failed = 0, skipped = 0;
      for (const auto& report : reports) {
        total += report.total_tests;
        failed += report.failed;
        skipped += report.skipped;
      }
      std::string json = std::format(
          "{{\"schema_version\":1,\"tests\":{},\"failed\":{},\"skipped\":{},\"suites\":[", total,
          failed, skipped);
      bool first_suite = true;
      for (const auto& report : reports) {
        if (!std::exchange(first_suite, false)) json += ',';
        json += "{\"name\":" + quote(report.name) + ",\"file\":" + quote(report.file.string()) +
                ",\"time\":" + std::to_string(report.time) + ",\"tests\":[";
        bool first_test = true;
        for (const auto& entry : report.testcases) {
          if (!std::exchange(first_test, false)) json += ',';
          json += "{\"name\":" + quote(entry.name) + ",\"file\":" + quote(entry.file) +
                  ",\"line\":" + std::to_string(entry.line) + ",\"status\":" +
                  quote(entry.failed ? "failed"
                                     : (!entry.skip_entries.empty() ? "skipped" : "passed")) +
                  ",\"time\":" + std::to_string(entry.time) + ",\"stdout\":" + quote(entry.sysout) +
                  ",\"stderr\":" + quote(entry.syserr);
          auto found = result_metadata.find(report.name + "." + entry.name);
          auto* metadata = found == result_metadata.end() ? nullptr : found->second;
          json += ",\"parameters\":" + quote(metadata ? metadata->parameters : "") +
                  ",\"tags\":" + (metadata ? json_strings(metadata->tags) : "[]") +
                  ",\"failures\":[";
          bool first_failure = true;
          for (const auto& failure : entry.failures) {
            if (!std::exchange(first_failure, false)) json += ',';
            json +=
                "{\"message\":" + quote(failure.message) + ",\"phase\":" + quote(failure.phase) +
                ",\"type\":" + quote(std::format("{}", failure.type)) +
                ",\"diagnostic\":" + quote(std::format("{}", failure.cdata)) + ",\"location\":" +
                (failure.cdata.location
                     ? "{\"file\":" + quote(failure.cdata.location->first) +
                           ",\"line\":" + std::to_string(failure.cdata.location->second) + "}"
                     : "null") +
                "}";
          }
          json += "],\"skip_reason\":" +
                  quote(entry.skip_entries.empty() ? "" : entry.skip_entries.front().message) +
                  ",\"properties\":[";
          bool first_property = true;
          for (const auto& property : entry.properties) {
            if (!std::exchange(first_property, false)) json += ',';
            json +=
                "{\"name\":" + quote(property.name) + ",\"value\":" + quote(property.value) + "}";
          }
          json += "]}";
        }
        json += "]}";
      }
      write_report(options.json_output, json + "]}\n");
    } catch (const std::exception& error) {
      std::println(std::cerr, "Cannot write JSON report {}: {}", options.json_output, error.what());
      status = 1;
    }
  }
  if (!options.xml_output.empty()) {
    try {
      std::string contents;
      if (reports.size() != 1) {
        unsigned tests = 0, failures = 0, skips = 0, errors = 0;
        double time = 0;
        for (const auto& report : reports) {
          tests += report.total_tests;
          failures += report.failed;
          skips += report.skipped;
          errors += report.errors;
          time += report.time;
        }
        contents = std::format(
            "<testsuites tests=\"{}\" failures=\"{}\" skipped=\"{}\" errors=\"{}\" time=\"{}\">",
            tests, failures, skips, errors, time);
      }
      for (const auto& report : reports) {
        suite_log = report;
        auto xml = xml_report();
        if (xml.starts_with("<?xml")) xml.erase(0, xml.find("?>") + 2);
        contents += xml;
      }
      if (reports.size() != 1) contents += "</testsuites>";
      write_report(options.xml_output, contents);
    } catch (const std::exception& error) {
      std::println(std::cerr, "Cannot write XML report {}: {}", options.xml_output, error.what());
      status = 1;
    }
  }
  if (options.brief) {
    unsigned tests = 0, failures = 0, skipped = 0;
    for (const auto& report : reports) {
      tests += report.total_tests;
      failures += report.failed;
      skipped += report.skipped;
    }
    std::println("Tests: {}, failures: {}, skipped: {}", tests, failures, skipped);
  }
  return status;
}
}  // namespace annotest
