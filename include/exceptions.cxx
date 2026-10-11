export module annotest:exceptions;

import std;
import :diagnostics;

namespace annotest {
// An exception class for test failures, to distinguish from other exceptions. It does not inherit
// std::exception to prevent the user from catching it as frequently
class assertion_error {
 public:
  assertion_error() = default;

  assertion_error(const assertion_error&) = default;
  assertion_error& operator=(const assertion_error&) = default;

  assertion_error(assertion_error&&) = default;
  assertion_error& operator=(assertion_error&&) = default;

  ~assertion_error() = default;

  assertion_error(const std::string_view message, bool recorded = false)
      : recorded_(recorded), message_(message), stacktrace_(capture_stacktrace()) {}

  std::string message() const {
    return std::format("Assertion failed with message \"{}\"", message_);
  }

  bool recorded() const { return recorded_; }

  const stacktrace_type& stacktrace() const { return stacktrace_; }

 private:
  bool recorded_ = false;
  std::string message_;
  stacktrace_type stacktrace_ = capture_stacktrace();
};
}  // namespace annotest