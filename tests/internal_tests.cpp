// Test private implementation behavior without exporting private exceptions or logs.
module annotest;
import std;

using namespace annotest;

// to_string uses define_static_string, so verify both specializations during
// constant evaluation rather than relying on runtime coverage counters.
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

extern "C++" int main(int argc, char** argv) {
  if (argc != 2) return 2;
  const std::string_view name = argv[1];
  if (name == "logger_without_test") {
    suite_log.reset();
    suite_log.add_failure({failure_types::expectation, "orphan", {.message = "orphan"}});
    require(suite_log.testcases.empty());
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
  } else if (name == "unrecorded_assertion") {
    suite_log.reset();
    suite_log.name = "InternalTests";
    require(execute_test(
                nullptr, "unrecorded", [](void*) { throw assertion_error{"manual"}; }, nullptr,
                nullptr) == 1);
    require(suite_log.testcases.back().failures.size() == 1);
    require(suite_log.testcases.back().failures[0].message ==
            "Assertion failed with message \"manual\"");
  } else if (name == "unrunnable_plan") {
    const discovered_test broken{.name = "broken"};
    runner_options options;
    std::vector<std::string> remaining;
    auto start = begin_suite("InternalTests", "internal_tests.cpp", false);
    require(execute_suite(nullptr, "InternalTests", std::span(&broken, 1), options, remaining,
                          start, nullptr, nullptr) == 1);
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
