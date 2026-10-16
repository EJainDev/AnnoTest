export module annotest:asserts;

import :exceptions;
import :contracts;
import :utils;
import :logger;

import std;

namespace annotest {
void numeric_equality_failure(std::format_args values, const std::source_location& location,
                              bool equal);
}

export namespace annotest {
void add_failure(std::string_view msg, const std::source_location& loc);

template <typename T, typename U>
  requires c_string_pair<T, U>
void assert_streq(T expected, U actual, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, false) != true) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, true, false);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void assert_strne(T expected, U actual, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, false) != false) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, false, false);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void assert_strcaseeq(T expected, U actual, std::string_view message = {},
                      std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, true) != true) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, true, true);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void assert_strcasene(T expected, U actual, std::string_view message = {},
                      std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, true) != false) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, false, true);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure(diagnostic, location);
  }
}

// Assert statements that can be called
void assert_equal(auto expected, auto actual,
                  const std::source_location& loc = std::source_location::current()) {
  if (expected != actual) {
    if constexpr (std::is_arithmetic_v<decltype(expected)> &&
                  std::is_arithmetic_v<decltype(actual)> &&
                  std::formattable<decltype(expected), char> &&
                  std::formattable<decltype(actual), char>) {
      numeric_equality_failure(std::make_format_args(expected, actual), loc, true);
    } else {
      add_failure(diagnostic_format("Assertion failed: expected {}, got {}", expected, actual) +
                      difference(expected, actual),
                  loc);
    }
  }
}

void assert_not_equal(auto expected, auto actual,
                      const std::source_location& loc = std::source_location::current()) {
  if (expected == actual) {
    if constexpr (std::is_arithmetic_v<decltype(expected)> &&
                  std::is_arithmetic_v<decltype(actual)> &&
                  std::formattable<decltype(expected), char> &&
                  std::formattable<decltype(actual), char>) {
      numeric_equality_failure(std::make_format_args(expected, actual), loc, false);
    } else {
      add_failure(
          diagnostic_format("Assertion failed: expected not equal to {}, got {}", expected, actual),
          loc);
    }
  }
}

void assert_true(auto value, const std::source_location& loc = std::source_location::current()) {
  if (!value) {
    add_failure(diagnostic_format("Assertion failed: expected true, got false for {}", value), loc);
  }
}

void assert_false(auto value, const std::source_location& loc = std::source_location::current()) {
  if (value) {
    add_failure(diagnostic_format("Assertion failed: expected false, got true for {}", value), loc);
  }
}

template <typename T, typename U, typename K = std::common_type_t<T, U>>
void assert_near(T expected, U actual, K tol = static_cast<K>(0.001),
                 const std::source_location& loc = std::source_location::current()) {
  if (!(std::abs(expected - actual) <= tol)) {
    add_failure(
        diagnostic_format("Assertion failed: expected {}, got {} within {}", expected, actual, tol),
        loc);
  }
}

void assert_less(auto a, auto b,
                 const std::source_location& loc = std::source_location::current()) {
  if (!(a < b)) {
    add_failure(diagnostic_format("Assertion failed: expected {} < {}, relation was false", a, b),
                loc);
  }
}

void assert_less_equal(auto a, auto b,
                       const std::source_location& loc = std::source_location::current()) {
  if (!(a <= b)) {
    add_failure(diagnostic_format("Assertion failed: expected {} <= {}, relation was false", a, b),
                loc);
  }
}

void assert_greater(auto a, auto b,
                    const std::source_location& loc = std::source_location::current()) {
  if (!(a > b)) {
    add_failure(diagnostic_format("Assertion failed: expected {} > {}, relation was false", a, b),
                loc);
  }
}

void assert_greater_equal(auto a, auto b,
                          const std::source_location& loc = std::source_location::current()) {
  if (!(a >= b)) {
    add_failure(diagnostic_format("Assertion failed: expected {} >= {}, relation was false", a, b),
                loc);
  }
}

void assert_contains(auto const& container, auto const& value,
                     const std::source_location& loc = std::source_location::current()) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    add_failure(diagnostic_format("Assertion failed: container does not contain value {}", value),
                loc);
  }
}

template <typename E = std::exception>
void assert_throws(auto func, const std::source_location& loc = std::source_location::current()) {
  try {
    func();
  } catch (const skip_error&) {
    throw;
  } catch (const E&) {
    return;
  } catch (...) {
    add_failure("Assertion failed: thrown exception type did not match expected", loc);
    return;
  }
  add_failure("Assertion failed: expected exception, none thrown", loc);
}

auto wrap(auto f, auto&&... args) {
  return [f = std::move(f),
          args = std::tuple{std::forward<decltype(args)>(args)...}]() mutable -> decltype(auto) {
    return std::apply(f, args);
  };
}

template <typename E>
void assert_throws_exact(auto f,
                         const std::source_location& loc = std::source_location::current()) {
  try {
    f();
  } catch (const skip_error&) {
    throw;
  } catch (const E& e) {
    if (typeid(e) != typeid(E)) {
      add_failure(diagnostic_format("Expected exact exception type {}, got {}", typeid(E).name(),
                                    typeid(e).name()),
                  loc);
    }
    return;
  } catch (...) {
    add_failure("Thrown exception type did not match expected", loc);
    return;
  }
  add_failure("Expected exception, none thrown", loc);
}

void assert_null(auto ptr, const std::source_location& loc = std::source_location::current()) {
  if (ptr != nullptr) {
    add_failure(diagnostic_format("Assertion failed: expected nullptr, got {}",
                                  static_cast<const void*>(ptr)),
                loc);
  }
}

template <typename Func>
auto assert_contract_violation(Func f,
                               const std::source_location& loc = std::source_location::current()) {
  contract_violation_occurred = false;
  auto check = [&] {
    bool occurred = std::exchange(contract_violation_occurred, false);
    if (!occurred) add_failure("Expected contract violation, but none was detected", loc);
  };
  if constexpr (std::is_void_v<std::invoke_result_t<Func>>) {
    f();
    check();
  } else {
    auto result = f();
    check();
    return result;
  }
}

template <typename Func>
auto assert_no_contract_violation(
    Func f, const std::source_location& loc = std::source_location::current()) {
  contract_violation_occurred = false;
  auto check = [&] {
    bool occurred = std::exchange(contract_violation_occurred, false);
    if (occurred) add_failure("Expected no contract violation, but one was detected", loc);
  };
  if constexpr (std::is_void_v<std::invoke_result_t<Func>>) {
    f();
    check();
  } else {
    auto result = f();
    check();
    return result;
  }
}

template <typename T, typename U>
void assert_equal(T&& first, U&& second, std::string_view message,
                  std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_not_equal(T&& first, U&& second, std::string_view message,
                      std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_not_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_less(T&& first, U&& second, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_less(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_less_equal(T&& first, U&& second, std::string_view message,
                       std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_less_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_greater(T&& first, U&& second, std::string_view message,
                    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_greater(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_greater_equal(T&& first, U&& second, std::string_view message,
                          std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_greater_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void assert_contains(T&& first, U&& second, std::string_view message,
                     std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_contains(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T>
void assert_true(T&& value, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_true(std::forward<T>(value), location);
}
template <typename T>
void assert_false(T&& value, std::string_view message,
                  std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_false(std::forward<T>(value), location);
}
template <typename T>
void assert_null(T&& value, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_null(std::forward<T>(value), location);
}
template <typename T, typename U, typename K>
void assert_near(T expected, U actual, K tolerance, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_near(expected, actual, tolerance, location);
}
template <typename E = std::exception, typename F>
void assert_throws(F&& callable, std::string_view message,
                   std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_throws<E>(std::forward<F>(callable), location);
}
template <typename E = std::exception, typename F>
void assert_throws_exact(F&& callable, std::string_view message,
                         std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_throws_exact<E>(std::forward<F>(callable), location);
}
template <typename F>
decltype(auto) assert_contract_violation(
    F&& callable, std::string_view message,
    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  return assert_contract_violation(std::forward<F>(callable), location);
}
template <typename F>
decltype(auto) assert_no_contract_violation(
    F&& callable, std::string_view message,
    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  return assert_no_contract_violation(std::forward<F>(callable), location);
}
}  // namespace annotest
