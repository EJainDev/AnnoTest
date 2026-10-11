export module annotest:logger;

import std;

import :exceptions;

import serial_xml;

namespace xml = serial_xml;

namespace annotest {
enum class failure_types {
  assertion,
  expectation,
  contract_violation,
  uncaught_exception,
  unknown
};

struct failure_cdata {
  std::optional<std::pair<std::string, std::size_t>> location;
  std::string message;
  std::optional<std::stacktrace> stacktrace = std::stacktrace::current();
};

struct failure_xml_log {
  [[= xml::attribute]] failure_types type;
  [[= xml::attribute]] std::string message;
  // SerialXML escapes raw text, including embedded CDATA terminators.
  [[ = xml::raw, = xml::no_unpack ]] failure_cdata cdata;
};

struct test_xml_log {
  [[ = xml::attribute, = xml::name{"classname"} ]] std::string test_class;
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] double time;
  [[= xml::skip]] bool failed = false;
  [[= xml::attribute]] unsigned int errors;

  [[= xml::name{"system-out"}]] std::string sysout;
  [[= xml::name{"system-err"}]] std::string syserr;

  [[ = xml::raw, = xml::iter{"failure", "failures"},
     = xml::exclude_on_empty ]] std::vector<failure_xml_log>
      failures;
};

inline std::mutex failures_mtx;

struct[[= xml::name{"testsuite"}]] suite_xml_log {
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] std::filesystem::path file;
  [[ = xml::attribute, = xml::name{"tests"} ]] unsigned int total_tests;
  [[ = xml::attribute, = xml::name{"failures"} ]] unsigned int failed = 0;
  [[= xml::attribute]] unsigned int skipped = 0;
  [[= xml::attribute]] unsigned int errors = 0;
  [[= xml::attribute]] double time = 0.0;
  [[= xml::attribute]] std::string timestamp;

  [[ = xml::raw, = xml::iter{"testcase", "testcases"},
     = xml::exclude_on_empty ]] std::vector<test_xml_log>
      testcases;

  void add_failure(const failure_xml_log& log) {
    std::lock_guard lock(failures_mtx);
    if (!testcases.empty()) {
      testcases.back().failures.emplace_back(log);
    }
  }

  void reset() {
    std::lock_guard lock(failures_mtx);
    name.clear();
    file.clear();
    total_tests = failed = skipped = errors = 0;
    time = 0;
    timestamp.clear();
    testcases.clear();
  }
};

inline suite_xml_log suite_log;

}  // namespace annotest

namespace std {
template <>
struct formatter<annotest::failure_cdata> : formatter<std::string> {
  constexpr auto format(const annotest::failure_cdata& cdata, auto& ctx) const {
    std::string formatted_cdata;

    if (cdata.location.has_value()) {
      formatted_cdata +=
          std::format("Location: {}:{}\n", cdata.location->first, cdata.location->second);
    }

    formatted_cdata += std::format("Message: {}\n", cdata.message);

    if (cdata.stacktrace.has_value()) {
      formatted_cdata += std::format("Stacktrace:\n{}\n", cdata.stacktrace.value());
    }

    return formatter<std::string>::format(formatted_cdata, ctx);
  }
};

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
      case annotest::failure_types::contract_violation:
        type_str = "contract_violation";
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
namespace annotest {
std::string xml_report() { return xml::to_xml(suite_log); }
}  // namespace annotest
