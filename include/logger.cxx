export module annotest:logger;

import std;
import :diagnostics;

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

// XML 1.0 accepts Unicode scalar values, but arbitrary captured bytes need
// a visible representation. Preserve valid UTF-8 and escape each rejected byte.
std::string xml_text(std::string_view text) {
  constexpr std::string_view hex = "0123456789abcdef";
  std::string result;
  result.reserve(text.size());
  auto escape_byte = [&](unsigned char byte) {
    result += "\\x";
    result += hex[byte >> 4];
    result += hex[byte & 15];
  };
  for (std::size_t i = 0; i < text.size();) {
    auto first = static_cast<unsigned char>(text[i]);
    std::size_t length = 1;
    unsigned int value = first;
    unsigned int minimum = 0;
    if (first >= 0xc2 && first <= 0xdf) {
      length = 2;
      value = first & 0x1f;
      minimum = 0x80;
    } else if (first >= 0xe0 && first <= 0xef) {
      length = 3;
      value = first & 0x0f;
      minimum = 0x800;
    } else if (first >= 0xf0 && first <= 0xf4) {
      length = 4;
      value = first & 0x07;
      minimum = 0x10000;
    } else if (first >= 0x80) {
      escape_byte(first);
      ++i;
      continue;
    }
    bool valid = length <= text.size() - i;
    if (valid) {
      for (std::size_t j = 1; j < length; ++j) {
        auto byte = static_cast<unsigned char>(text[i + j]);
        if ((byte & 0xc0) != 0x80) {
          valid = false;
          break;
        }
        value = (value << 6) | (byte & 0x3f);
      }
    }
    valid = valid && value >= minimum && value <= 0x10ffff && !(value >= 0xd800 && value <= 0xdfff);
    const bool xml_character =
        value == 9 || value == 10 || value == 13 || (value >= 0x20 && value <= 0xd7ff) ||
        (value >= 0xe000 && value <= 0xfffd) || (value >= 0x10000 && value <= 0x10ffff);
    if (valid && xml_character) {
      result.append(text.substr(i, length));
      i += length;
    } else {
      escape_byte(first);
      ++i;
    }
  }
  return result;
}

struct failure_cdata {
  std::optional<std::pair<std::string, std::size_t>> location;
  std::string message;
  std::optional<stacktrace_type> stacktrace = capture_stacktrace();
  bool sanitize_xml = false;
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

std::string xml_path(const std::filesystem::path& path) {
  const auto bytes = path.u8string();
  return xml_text(std::string(bytes.begin(), bytes.end()));
}

struct[[= xml::name{"testsuite"}]] suite_xml_log {
  [[= xml::attribute]] std::string name;
  [[ = xml::attribute, = xml::format{xml_path} ]] std::filesystem::path file;
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

    if (cdata.sanitize_xml) formatted_cdata = annotest::xml_text(formatted_cdata);
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
std::string xml_report() {
  auto report = suite_log;
  report.name = xml_text(report.name);
  report.timestamp = xml_text(report.timestamp);
  for (auto& test : report.testcases) {
    test.test_class = xml_text(test.test_class);
    test.name = xml_text(test.name);
    test.sysout = xml_text(test.sysout);
    test.syserr = xml_text(test.syserr);
    for (auto& failure : test.failures) {
      failure.message = xml_text(failure.message);
      failure.cdata.sanitize_xml = true;
    }
  }
  return xml::to_xml(report);
}
}  // namespace annotest
