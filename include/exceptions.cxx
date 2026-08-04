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

// Intended to be used by the user to add a stacktrace to an exception and throw it. It is a
// container for an exception, not an exception itself. Although user code may already have a
// stacktrace, the library cannot identify that, so this wrapper must be used.
template <typename E>
class error {
 public:
  error(const E& error, const std::stacktrace& stacktrace)
      : error_(error), stacktrace_(stacktrace) {}

  const E& exception() const { return error_; }
  const std::stacktrace& stacktrace() const { return stacktrace_; }

 private:
  E error_;
  std::stacktrace stacktrace_;
};
}  // namespace annotest