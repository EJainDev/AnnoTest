export module annotest:logger;

import std;

import :exceptions;

import serial_xml;

namespace xml = serial_xml;

namespace annotest {
enum class log_formats { txt, json, xml };

log_formats log_format;

struct active_test {
 public:
  active_test() = default;

  active_test(const active_test& other) = default;
  active_test& operator=(const active_test& other) = default;

  active_test(active_test&& other) = default;
  active_test& operator=(active_test&& other) = default;

  ~active_test() = default;

  void start() { start_time = std::chrono::steady_clock::now(); }
  void end() { end_time = std::chrono::steady_clock::now(); }

  void catch_exception(const std::exception& e) {
    end();
    uncaught_error = e;
    failed = true;
  }
  void catch_exception(const assertion_error& e) {
    end();
    assert_error = e;
    failure_trace = e.stacktrace();
    failed = true;
  }

  void catch_exception() {
    end();
    failed = true;
  }

  void fail() { failed = true; }

  std::string_view name;

  std::chrono::steady_clock::time_point start_time;
  std::chrono::steady_clock::time_point end_time;

  std::optional<std::exception> uncaught_error;
  std::optional<assertion_error> assert_error;
  std::optional<std::stacktrace> failure_trace;

  bool failed = false;
  bool is_before_each_failure = false;

  std::vector<std::pair<std::string, std::stacktrace>> failed_expectations;
};
}  // namespace annotest

namespace std {
template <>
struct formatter<::annotest::active_test> : formatter<string> {
  constexpr auto format(const ::annotest::active_test& test, auto& ctx) const {
    string formatted_test;

    if (::annotest::log_format == ::annotest::log_formats::txt) {
      formatted_test = ::std::format("Test: {} ", test.name);

      if (test.failed) {
        if (test.uncaught_error.has_value()) {
          formatted_test += ::std::format(
              "failed with uncaught exception of type {} and message \"{}\". Stacktrace: {}",
              typeid(test.uncaught_error).name(), test.uncaught_error->what(), test.uncaught_error);
        } else if (test.assert_error.has_value()) {
          formatted_test +=
              ::std::format("failed with assertion assertion_error: {}. Stacktrace: {}",
                            test.assert_error->message(), test.assert_error->stacktrace());
        } else if (test.failure_trace.has_value()) {
          formatted_test += ::std::format("failed with unknown exception. Stacktrace: {}",
                                          test.failure_trace.value());
        } else {
          formatted_test += "failed with unknown exception";
        }
      } else {
        formatted_test += "passed";
      }

      formatted_test += ::std::format(
          " in {} seconds",
          ::std::chrono::duration_cast<::std::chrono::milliseconds>(test.end_time - test.start_time)
                  .count() /
              1'000.0);
    } else if (::annotest::log_format == ::annotest::log_formats::json) {
      formatted_test = ::std::format(
          "{{\"name\": \"{}\", \"passed\": {}, \"duration\": {:.3f}", test.name, !test.failed,
          ::std::chrono::duration_cast<::std::chrono::milliseconds>(test.end_time - test.start_time)
                  .count() /
              1'000.0);

      if (test.failed) {
        formatted_test += ::std::format(
            ", \"error\": \"{}\", \"stacktrace\": \"{}\"",
            test.uncaught_error.has_value()
                ? ::std::format("Uncaught exception of type {} with message: {}",
                                typeid(test.uncaught_error).name(), test.uncaught_error->what())
                : (test.assert_error.has_value()
                       ? ::std::format("Assertion error: {}", test.assert_error->message())
                       : "Unknown exception"),
            test.failure_trace.has_value() ? ::std::format("{}", test.failure_trace.value())
                                           : "No stacktrace available");
      }
    } else if (::annotest::log_format == ::annotest::log_formats::xml) {
      const auto duration =
          ::std::chrono::duration_cast<::std::chrono::milliseconds>(test.end_time - test.start_time)
              .count() /
          1'000.0;

      auto escape_xml = [](::std::string_view text) {
        ::std::string escaped;
        escaped.reserve(text.size());

        for (const char ch : text) {
          switch (ch) {
            case '&':
              escaped += "&amp;";
              break;
            case '<':
              escaped += "&lt;";
              break;
            case '>':
              escaped += "&gt;";
              break;
            case '"':
              escaped += "&quot;";
              break;
            case '\'':
              escaped += "&apos;";
              break;
            default:
              escaped += ch;
              break;
          }
        }

        return escaped;
      };

      formatted_test = ::std::format(
          "<test>\n"
          "  <name>{}</name>\n"
          "  <passed>{}</passed>\n"
          "  <duration>{:.3f}</duration>",
          escape_xml(test.name), !test.failed, duration);

      if (test.failed) {
        const auto error =
            test.uncaught_error.has_value()
                ? ::std::format("Uncaught exception of type {} with message: {}",
                                typeid(test.uncaught_error).name(), test.uncaught_error->what())
                : (test.assert_error.has_value()
                       ? ::std::format("Assertion error: {}", test.assert_error->message())
                       : "Unknown exception");
        const auto stacktrace = test.failure_trace.has_value()
                                    ? ::std::format("{}", test.failure_trace.value())
                                    : "No stacktrace available";

        formatted_test += ::std::format(
            "\n  <error>{}</error>\n"
            "  <stacktrace>{}</stacktrace>",
            escape_xml(error), escape_xml(stacktrace));
      }

      formatted_test += "\n</test>";
    }

    return ::std::formatter<string>::format(formatted_test, ctx);
  }
};
}  // namespace std

namespace annotest {
inline active_test active_test_instance;

class logger {
 public:
  void consume() { tests.emplace_back(std::move(active_test_instance)); }

  std::string write() {
    // std::string result;
    // if (log_format == log_formats::txt) {
    //   for (const auto& test : tests) {
    //     result += std::format("{}\n", test);
    //   }
    // } else if (log_format == log_formats::json) {
    //   result += std::format("{{\n  \"tests\": [\n");
    //   for (std::size_t i = 0; i < tests.size(); ++i) {
    //     result += std::format("    {}", tests[i]);
    //     if (i < tests.size() - 1) {
    //       result += ",\n";
    //     } else {
    //       result += "\n";
    //     }
    //   }
    //   result += "  ]\n}";
    // } else if (log_format == log_formats::xml) {
    //   result += std::format("<tests>\n");
    //   for (const auto& test : tests) {
    //     result += std::format("  {}\n", test);
    //   }
    //   result += "</tests>";
    // }
    // return result;
    return "";
  }

 private:
  std::vector<active_test> tests;
};

enum class failure_types { assertion, expectation, uncaught_exception, unknown };

struct failure_xml_log {
  [[= xml::attribute]] failure_types type;
  std::string message;
  std::stacktrace stacktrace;
};

struct test_xml_log {
  [[ = xml::attribute, = xml::name{"class"} ]] std::string test_class;
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] std::chrono::steady_clock::time_point start_time;
  [[= xml::attribute]] std::chrono::milliseconds time;
  [[= xml::attribute]] std::chrono::steady_clock::time_point end_time;
  [[= xml::attribute]] bool failed;
  [[= xml::attribute]] std::string status;

  [[= xml::iter{"failure", "failures"}]] std::vector<failure_xml_log> failures;
};

struct[[= xml::name{"testsuite"}]] xml_log {
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] std::filesystem::path file;
  [[ = xml::attribute, = xml::name{"tests"} ]] unsigned int total_tests;
  [[= xml::attribute]] std::size_t failed;
  [[= xml::attribute]] std::size_t disabled;
  [[= xml::attribute]] std::chrono::milliseconds time;

  [[= xml::iter{"testcase", "testcases"}]] std::vector<test_xml_log> testcases;
};
}  // namespace annotest

namespace std {
template <>
struct formatter<annotest::failure_types> : formatter<std::string> {
  constexpr auto format(const annotest::failure_types& type, auto& ctx) const {
    std::string type_str;
    switch (type) {
      case annotest::failure_types::assertion:
        type_str = "assertion";
        break;
      case annotest::failure_types::expectation:
        type_str = "expectation";
        break;
      case annotest::failure_types::uncaught_exception:
        type_str = "uncaught_exception";
        break;
      case annotest::failure_types::unknown:
        type_str = "unknown";
        break;
    }
    return formatter<std::string>::format(type_str, ctx);
  }
};
}  // namespace std