// Test private implementation behavior without exporting private exceptions or logs.
module annotest;
import std;

using namespace annotest;

// Verify constant evaluation across signed and unsigned conversion limits.
static_assert(annotest::to_string(0) == "0");
static_assert(annotest::to_string(42) == "42");
static_assert(annotest::to_string(-42) == "-42");
static_assert(annotest::to_string(std::numeric_limits<std::int64_t>::min()) ==
              "-9223372036854775808");
static_assert(annotest::to_string(std::numeric_limits<std::int64_t>::max()) ==
              "9223372036854775807");
static_assert(annotest::to_string(0u) == "0");
static_assert(annotest::to_string(42u) == "42");
static_assert(annotest::to_string(std::numeric_limits<std::uint64_t>::max()) ==
              "18446744073709551615");

void require(bool result) {
  if (!result) throw std::runtime_error("internal invariant failed");
}

struct BasicDiscoveryParent {
  [[= before_each{}]] void parent_before() {}
  [[= after_each{}]] void parent_after() {}
  [[= test{}]] void inherited() {}
};
struct BasicDiscovery : BasicDiscoveryParent {
  [[= before_each{}]] void before(test_context&) {}
  [[= after_each{}]] void after(int = 0) {}
  [[= test{}]] void inherited() {}
  [[= test{}]] void first(int = 0) {}
  [[= test{.disabled = true, .repetitions = 3}]] void second(test_context&) {}
  [[= test{}]] static void third() {}
};
static_assert(has_basic_annotations(^^BasicDiscovery));
static_assert([] {
  constexpr auto basic = get_tests<^^BasicDiscovery>();
  constexpr auto extended = get_extended_tests<^^BasicDiscovery>();
  if (std::string_view(basic.suite.name) != extended.suite.name ||
      basic.suite.suite != extended.suite.suite || basic.tests.size() != 4 ||
      basic.tests.size() != extended.tests.size() ||
      basic.suite.has_before_each != extended.suite.has_before_each ||
      basic.suite.has_after_each != extended.suite.has_after_each ||
      basic.suite.before_each != extended.suite.before_each ||
      basic.suite.after_each != extended.suite.after_each) return false;
  if (!std::ranges::equal(std::span{basic.suite.before_hooks.data, basic.suite.before_hooks.size},
                          std::span{extended.suite.before_hooks.data, extended.suite.before_hooks.size}) ||
      !std::ranges::equal(std::span{basic.suite.after_hooks.data, basic.suite.after_hooks.size},
                          std::span{extended.suite.after_hooks.data, extended.suite.after_hooks.size}))
    return false;
  for (std::size_t index = 0; index < basic.tests.size(); ++index) {
    const auto& first = basic.tests[index];
    const auto& second = extended.tests[index];
    if (first.test != second.test || first.raw_test != second.raw_test ||
        std::string_view(first.name) != second.name || first.disabled != second.disabled ||
        first.repetitions != second.repetitions || first.parameterized != second.parameterized ||
        first.runtime_parameters != second.runtime_parameters || first.dynamic != second.dynamic ||
        first.timeout_seconds != second.timeout_seconds || first.tags.size != second.tags.size)
      return false;
  }
  return true;
}());

extern "C++" int main(int argc, char** argv) {
  if (argc != 2) return 2;
  auto check_parameters = [](const auto& values) {
    require(print_parameters(values) == std::format("{}", values));
  };
  check_parameters(std::tuple{});
  check_parameters(std::tuple{true, false, -42, 42u});
  check_parameters(std::tuple{std::numeric_limits<std::int64_t>::min(),
                               std::numeric_limits<std::uint64_t>::max()});
  check_parameters(std::tuple{static_cast<signed char>(-7), static_cast<unsigned char>(9)});
  check_parameters(std::tuple{-0.0, 2.5f, 3.25L});
  check_parameters(std::tuple{std::numeric_limits<double>::infinity(),
                               std::numeric_limits<double>::quiet_NaN()});
  check_parameters(std::tuple{'\n', std::string("quoted \"text\"\n")});
  const std::string_view name = argv[1];
  if (name == "logger_without_test") {
    suite_log.reset();
    suite_log.add_failure({failure_types::expectation, "orphan", {.message = "orphan"}});
    require(suite_log.testcases.empty());
    require(current_failures().empty());
    test_context context;
    auto* saved_context = active_context;
    active_context = &context;
    bool rejected = false;
    try {
      record_property("orphan", "value");
    } catch (const std::logic_error& error) {
      rejected = std::string_view(error.what()) == "No active test";
    }
    active_context = saved_context;
    require(rejected);
  } else if (name == "unicode_string_boundaries") {
    const std::string expected = "\x7f\xc2\x80\xdf\xbf\xe0\xa0\x80\xef\xbf\xbf"
                                 "\xf0\x90\x80\x80\xf4\x8f\xbf\xbf";
    require(string_bytes<char32_t>(U"\x7f\u0080\u07ff\u0800\uffff\U00010000\U0010ffff") == expected);
    require(string_bytes<char16_t>(u"\x7f\u0080\u07ff\u0800\uffff\U00010000\U0010ffff") == expected);
    require(string_bytes<wchar_t>(L"\x7f\u0080\u07ff\u0800\uffff\U00010000\U0010ffff") == expected);
    const std::array<char32_t, 3> invalid{0xd800, 0xdfff, 0x110000};
    require(string_bytes<char32_t>({invalid.data(), invalid.size()}) ==
            "\\u{d800}\\u{dfff}\\u{110000}");
    require(string_bytes<char16_t>(u"\xd800x\xdfff") == "\\u{d800}x\\u{dfff}");
    require(string_bytes<char16_t>(u"\xd800") == "\\u{d800}");
    const std::array<char32_t, 2> zero{0, 'A'};
    require(string_bytes<char32_t>({zero.data(), zero.size()}) == std::string("\0A", 2));
  } else if (name == "optional_failure_fields") {
    const failure_cdata data{
        .location = std::nullopt, .message = "plain", .stacktrace = std::nullopt};
    require(std::format("{}", data) == "Message: plain\n");
  } else if (name == "failure_type_formatting") {
    require(std::format("{}", failure_types::assertion) == "assertion");
    require(std::format("{}", failure_types::expectation) == "expectation");
    require(std::format("{}", failure_types::contract_violation) == "contract_violation");
    require(std::format("{}", failure_types::uncaught_exception) == "uncaught_exception");
    require(std::format("{}", failure_types::unknown) == "unknown");
    require(std::format("{}", static_cast<failure_types>(99)).empty());
  } else if (name == "xml_snapshot_preserves_raw_log") {
    suite_log.reset();
    auto saved = suite_log;
    suite_log.testcases.emplace_back();
    auto bytes = std::string("\x00\x80", 2);
    suite_log.name = bytes;
    suite_log.timestamp = bytes;
    const auto source_path = std::filesystem::path(u8"source-é中🙂\x01.cpp");
    suite_log.file = source_path;
    auto& entry = suite_log.testcases.back();
    entry.name = bytes;
    entry.test_class = bytes;
    entry.sysout = bytes;
    entry.syserr = bytes;
    entry.failures.push_back({failure_types::expectation,
                              bytes,
                              {.location = std::pair{std::string("path\x01"), std::size_t(7)},
                               .message = bytes,
                               .stacktrace = std::nullopt}});
    const auto serialized = xml_report();
    const bool unchanged =
        suite_log.name == bytes && suite_log.timestamp == bytes && suite_log.file == source_path &&
        entry.name == bytes && entry.test_class == bytes && entry.sysout == bytes &&
        entry.syserr == bytes && entry.failures.back().message == bytes &&
        entry.failures.back().cdata.message == bytes && !entry.failures.back().cdata.sanitize_xml;
    suite_log = std::move(saved);
    require(unchanged);
    require(serialized.find("\\x00\\x80") != std::string::npos);
    require(serialized.find("source-é中🙂\\x01.cpp") != std::string::npos);
    require(serialized.find("Location: path\\x01:7") != std::string::npos);
  } else if (name == "unrecorded_assertion") {
    suite_log.reset();
    suite_log.name = "InternalTests";
    require(execute_test(
                nullptr, "unrecorded", [](void*) { throw assertion_error{"manual"}; }, nullptr,
                nullptr) == 1);
    require(suite_log.testcases.back().failures.size() == 1);
    require(suite_log.testcases.back().failures[0].message ==
            "Assertion failed with message \"manual\"");
  } else if (name == "grouped_assertion_outcomes") {
    std::function<void()> block;
    auto check = [&](int status, std::size_t failures, failure_types kind) {
      suite_log.reset();
      suite_log.name = "InternalTests";
      require(execute_test(
                  &block, "grouped", [](void* data) {
                    run_assertion_block(*static_cast<std::function<void()>*>(data));
                  }, nullptr, nullptr) == status);
      const auto& result = suite_log.testcases.back();
      require(result.failures.size() == failures);
      if (failures) require(result.failures[0].type == kind);
    };
    block = [] {};
    check(0, 0, failure_types::unknown);
    block = [] { assert_true(false); };
    check(1, 1, failure_types::assertion);
    block = [] { throw assertion_error{"manual group failure"}; };
    check(1, 1, failure_types::assertion);
    block = [] { throw std::runtime_error("group exception"); };
    check(1, 1, failure_types::uncaught_exception);
    block = [] { throw 42; };
    check(1, 1, failure_types::unknown);
    block = [] { skip("group skip"); };
    check(0, 0, failure_types::unknown);
    require(suite_log.testcases.back().skip_entries.size() == 1);
  } else if (name == "unrunnable_plan") {
    execution_suite broken{.name = "InternalTests", .file = "internal_tests.cpp"};
    broken.cases.push_back({.name = "broken", .base_name = "broken"});
    require(run_plan({broken}, runner_options{}, 0, nullptr) == 1);
  } else if (name == "null_argv") {
    const auto options = parse_options(2, nullptr);
    require(options.should_exit && options.exit_code == 1);
  } else if (name == "null_argument") {
    char program[] = "tests";
    char* arguments[]{program, nullptr};
    const auto options = parse_options(2, arguments);
    require(options.should_exit && options.exit_code == 1);
  } else if (name == "null_xml_path" || name == "null_test_name") {
    char program[] = "tests";
    char xml[] = "--xml-output", test_name[] = "--test-name";
    char* arguments[]{program, name == "null_xml_path" ? xml : test_name, nullptr};
    const auto options = parse_options(3, arguments);
    require(options.should_exit && options.exit_code == 1);
  } else if (name == "help_without_program") {
    char help[] = "--help";
    char* arguments[]{nullptr, help};
    const auto options = parse_options(2, arguments);
    require(options.should_exit && options.exit_code == 0);
  } else if (name == "empty_arguments") {
    const auto options = parse_options(0, nullptr);
    require(!options.should_exit && options.test_names.empty());
  } else {
    return 2;
  }
  std::println("verified: {}", name);
}
