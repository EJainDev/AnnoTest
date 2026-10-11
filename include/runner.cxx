module;

#include <unistd.h>

#include <cstdio>

module annotest;

import std;

namespace annotest {

// Temporary files let large writes complete without full pipes or background reader threads.
class std_streams {
 public:
  std_streams() {
    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);
    out = std::tmpfile();
    err = std::tmpfile();
    saved_out = dup(STDOUT_FILENO);
    saved_err = dup(STDERR_FILENO);
    if (!out || !err || saved_out < 0 || saved_err < 0) {
      restore();
      throw std::runtime_error("Cannot capture test output");
    }
    if (dup2(fileno(out), STDOUT_FILENO) < 0 || dup2(fileno(err), STDERR_FILENO) < 0) {
      restore();
      throw std::runtime_error("Cannot redirect test output");
    }
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
      dup2(saved_out, STDOUT_FILENO);
      close(saved_out);
      saved_out = -1;
    }
    if (saved_err >= 0) {
      dup2(saved_err, STDERR_FILENO);
      close(saved_err);
      saved_err = -1;
    }
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
};

void record_exception(const std::string& phase) {
  try {
    throw;
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
                 test_callback after) {
  contract_violation_occurred = false;
  suite_log.testcases.emplace_back(
      test_xml_log{.test_class = suite_log.name, .name = std::string(name)});
  auto start = std::chrono::steady_clock::now();
  {
    std_streams streams;
    bool ready = true;
    if (before) {
      try {
        before(context);
      } catch (...) {
        record_exception("before_each");
        ready = false;
      }
    }
    if (ready) {
      try {
        test(context);
      } catch (...) {
        record_exception("test");
      }
    }
    if (after) {
      try {
        after(context);
      } catch (...) {
        record_exception("after_each");
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
  std::println("{}: {}", name, result.failed ? "Failed" : "Passed");
  for (const auto& failure : result.failures) std::println(std::cerr, "{}", failure.cdata);
  return result.failed ? 1 : 0;
}

runner_options parse_options(int argc, char** argv) {
  runner_options options;
  bool show_help = false;
  bool xml_seen = false;
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
    std::string_view arg = argv[i];
    if (arg == "--") {
      if (i + 1 < argc) error("Positional arguments are not supported");
      break;
    }
    if (arg.starts_with("--")) {
      auto equal = arg.find('=');
      auto name = arg.substr(0, equal);
      bool has_value = equal != std::string_view::npos;
      auto value = has_value ? arg.substr(equal + 1) : std::string_view{};
      if (name == "--list" || name == "--help") {
        if (has_value) {
          error(std::format("{} does not accept a value", name));
        } else if (name == "--list") {
          options.list_tests = true;
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

  if (show_help && !options.should_exit) {
    auto program = argc > 0 && argv && argv[0] ? argv[0] : "tests";
    std::println(
        "AnnoTest Based Tests\n"
        "Usage: {} [OPTIONS]\n\n"
        "Options:\n"
        "  -h, --help                  Show this help and exit\n"
        "  -l, --list                  List all available tests\n"
        "  -t, --test-name NAME...     Run fully qualified test names (repeatable)\n"
        "      --xml-output FILE      Write a suite XML report to this file",
        program);
    options.should_exit = true;
  }
  return options;
}

bool select_parameterizations(const std::string& full_name, bool parameterized, int count,
                              const std::vector<std::string>& requested_names,
                              std::vector<std::string>& remaining_names,
                              std::vector<int>& targets) {
  if (requested_names.empty()) {
    targets.push_back(-1);
    return true;
  }
  for (const auto& requested : requested_names) {
    if (requested == full_name) {
      targets = {-1};
      std::erase(remaining_names, requested);
    } else if (parameterized && requested.starts_with(full_name + ".")) {
      auto suffix = std::string_view(requested).substr(full_name.size() + 1);
      int index = 0;
      auto [end, ec] = std::from_chars(suffix.data(), suffix.data() + suffix.size(), index);
      if (ec != std::errc{} || end != suffix.data() + suffix.size() || index < 1 || index > count) {
        std::println(std::cerr, "Invalid parameterization: {}", requested);
        return false;
      }
      if (std::ranges::find(targets, -1) == targets.end() &&
          std::ranges::find(targets, index - 1) == targets.end()) {
        targets.push_back(index - 1);
      }
      std::erase(remaining_names, requested);
    }
  }
  return true;
}

void list_tests(std::string_view suite_name, std::span<const discovered_test> tests) {
  std::println("{}.\n", suite_name);
  for (const auto& test : tests) {
    if (test.parameter_count > 0) {
      for (int i = 1; i <= test.parameter_count; ++i) {
        std::println("\t{}.{}", test.name, i);
      }
    } else {
      std::println("\t{}", test.name);
    }
  }
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

  if (!filtered) {
    std::println("Running tests in suite {}", suite_name);
  } else {
    std::println("Running specified tests in suite {}", suite_name);
  }

  return suite_start;
}

int execute_suite(void* context, std::string_view suite_name,
                  std::span<const discovered_test> tests, const runner_options& options,
                  std::vector<std::string>& remaining_names,
                  std::chrono::steady_clock::time_point suite_start, test_callback before,
                  test_callback after) {
  const auto& test_names = options.test_names;
  const auto& xml_output = options.xml_output;
#if defined(__linux__)
  static constexpr auto os = OS::Linux;
#elif defined(_WIN32)
  static constexpr auto os = OS::Windows;
#elif defined(__APPLE__)
  static constexpr auto os = OS::Mac;
#else
  static constexpr auto os = OS::Unknown;
#endif

  int status_code = 0;

  std::string prefix = std::string(suite_name) + '.';

  for (const auto& test : tests) {
    contract_violation_occurred = false;  // Reset contract flag before each test

    const auto test_name = test.name;
    const bool is_parameterized = test.parameterized;
    const int parameter_count = test.parameter_count;
    std::vector<int> parameterize_targets;
    auto full_name = prefix + test_name;
    if (!select_parameterizations(full_name, is_parameterized, parameter_count, test_names,
                                  remaining_names, parameterize_targets))
      return 1;
    if (parameterize_targets.empty()) continue;

    // Skip disabled tests
    if (test.disabled) {
      suite_log.skipped +=
          is_parameterized && parameterize_targets.size() == 1 && parameterize_targets.front() == -1
              ? parameter_count
              : parameterize_targets.size();
      std::println("Skipping disabled test: {}", test_name);
      continue;
    }

    // A catch-all flag representing whether to stop the test early for OS requirements
    bool stop = false;

    // Check for required operating systems
    if (test.has_required_os) {
      stop = true;
      for (auto required_os : test.required_os) {
        if (required_os == os) {
          std::println("Current OS {} matches required OS {} for test {}",
                       enum_to_string(required_os), enum_to_string(os), test_name);
          stop = false;
          break;
        }
      }
    }

    if (stop) {
      suite_log.skipped +=
          is_parameterized && parameterize_targets.size() == 1 && parameterize_targets.front() == -1
              ? parameter_count
              : parameterize_targets.size();
      continue;
    }

    // Check for disallowed operating systems
    for (auto disallowed_os : test.disallowed_os) {
      if (disallowed_os == os) {
        std::println("Current OS {} is disallowed for test {}, skipping",
                     enum_to_string(disallowed_os), test_name);
        stop = true;
        break;
      }
    }

    if (stop) {
      suite_log.skipped +=
          is_parameterized && parameterize_targets.size() == 1 && parameterize_targets.front() == -1
              ? parameter_count
              : parameterize_targets.size();
      continue;
    }

    for (int index : parameterize_targets) {
      if (test.run) {
        if (test.run(context, index) == 1) status_code = 1;
      } else if (test.body) {
        if (execute_test(context, test_name, test.body, before, after) == 1) status_code = 1;
      } else {
        std::println(
            "Warning: test `{}` did not execute because it has required arguments that were not "
            "given via a parameterization annotation.",
            test_name);
      }
    }
  }

  if (!remaining_names.empty()) {
    status_code = 1;
    std::println(std::cerr, "Warning: The following test names were not found: {}",
                 remaining_names);
  }

  suite_log.total_tests = suite_log.testcases.size() + suite_log.skipped;
  suite_log.time =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - suite_start).count();
  if (!xml_output.empty()) {
    try {
      std::ofstream report(xml_output);
      report << xml_report();
      report.flush();
      if (!report) throw std::runtime_error("Cannot write report file");
    } catch (const std::exception& e) {
      std::println(std::cerr, "Cannot write XML report {}: {}", xml_output, e.what());
      return 1;
    }
  }
  return status_code;
}

}  // namespace annotest
