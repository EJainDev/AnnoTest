export module annotest:exceptions;

import std;

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

  assertion_error(const std::string_view message)
      : message_(std::move(message)), stacktrace_(std::stacktrace::current()) {}

  std::string message() const {
    return std::format("Assertion failed with message \"{}\"", message_);
  }

  const std::stacktrace& stacktrace() const { return stacktrace_; }

 private:
  std::string message_;
  std::stacktrace stacktrace_ = std::stacktrace::current();
};
}  // namespace annotest