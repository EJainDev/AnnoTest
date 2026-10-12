export module annotest:expects;

import :contracts;
import :exceptions;
import :utils;
import :logger;

import std;

export namespace annotest {
inline void add_failure_expect(const std::string_view msg, const std::source_location& loc) {
  suite_log.add_failure(failure_xml_log{
      failure_types::expectation, std::string{msg},
      failure_cdata{std::make_pair(loc.file_name(), loc.line()), std::string{msg}}});
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void expect_streq(T expected, U actual, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, false) != true) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, true, false);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure_expect(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void expect_strne(T expected, U actual, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, false) != false) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, false, false);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure_expect(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void expect_strcaseeq(T expected, U actual, std::string_view message = {},
                      std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, true) != true) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, true, true);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure_expect(diagnostic, location);
  }
}

template <typename T, typename U>
  requires c_string_pair<T, U>
void expect_strcasene(T expected, U actual, std::string_view message = {},
                      std::source_location location = std::source_location::current()) {
  using Char = c_string_char<T, U>;
  if (c_strings_equal<Char>(expected, actual, true) != false) {
    auto diagnostic = string_comparison_diagnostic(expected, actual, false, true);
    if (!message.empty()) diagnostic += "\n" + std::string(message);
    add_failure_expect(diagnostic, location);
  }
}

// Assert statements that can be called
void expect_equal(auto expected, auto actual,
                  const std::source_location& loc = std::source_location::current()) {
  if (expected != actual) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected {}, got {}", expected, actual) +
            difference(expected, actual),
        loc);
  }
}

void expect_not_equal(auto expected, auto actual,
                      const std::source_location& loc = std::source_location::current()) {
  if (expected == actual) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected not equal to {}, got {}", expected, actual),
        loc);
  }
}

void expect_true(auto value, const std::source_location& loc = std::source_location::current()) {
  if (!value) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected true, got false for {}", value), loc);
  }
}

void expect_false(auto value, const std::source_location& loc = std::source_location::current()) {
  if (value) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected false, got true for {}", value), loc);
  }
}

template <typename T, typename U, typename K = std::common_type_t<T, U>>
void expect_near(T expected, U actual, K tol = static_cast<K>(0.001),
                 const std::source_location& loc = std::source_location::current()) {
  if (!(std::abs(expected - actual) <= tol)) {
    add_failure_expect(diagnostic_format("Expectation failed: expected {}, got {} within {}",
                                         expected, actual, tol),
                       loc);
  }
}

void expect_less(auto a, auto b,
                 const std::source_location& loc = std::source_location::current()) {
  if (!(a < b)) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected {} < {}, relation was false", a, b), loc);
  }
}

void expect_less_equal(auto a, auto b,
                       const std::source_location& loc = std::source_location::current()) {
  if (!(a <= b)) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected {} <= {}, relation was false", a, b), loc);
  }
}

void expect_greater(auto a, auto b,
                    const std::source_location& loc = std::source_location::current()) {
  if (!(a > b)) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected {} > {}, relation was false", a, b), loc);
  }
}

void expect_greater_equal(auto a, auto b,
                          const std::source_location& loc = std::source_location::current()) {
  if (!(a >= b)) {
    add_failure_expect(
        diagnostic_format("Expectation failed: expected {} >= {}, relation was false", a, b), loc);
  }
}

void expect_contains(auto const& container, auto const& value,
                     const std::source_location& loc = std::source_location::current()) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    add_failure_expect(
        diagnostic_format("Expectation failed: container does not contain value {}", value), loc);
  }
}

template <typename E = std::exception>
void expect_throws(auto func, const std::source_location& loc = std::source_location::current()) {
  try {
    func();
  } catch (const skip_error&) {
    throw;
  } catch (const E&) {
    return;
  } catch (...) {
    add_failure_expect("Expectation failed: thrown exception type did not match expected", loc);
    return;
  }
  add_failure_expect("Expectation failed: expected exception, none thrown", loc);
}

template <typename E>
void expect_throws_exact(auto f,
                         const std::source_location& loc = std::source_location::current()) {
  try {
    f();
  } catch (const skip_error&) {
    throw;
  } catch (const E& e) {
    if (typeid(e) != typeid(E)) {
      add_failure_expect(diagnostic_format("Expected exact exception type {}, got {}",
                                           typeid(E).name(), typeid(e).name()),
                         loc);
    }
    return;
  } catch (...) {
    add_failure_expect("Thrown exception type did not match expected", loc);
    return;
  }
  add_failure_expect("Expected exception, none thrown", loc);
}

void expect_null(auto ptr, const std::source_location& loc = std::source_location::current()) {
  if (ptr != nullptr) {
    add_failure_expect(diagnostic_format("Expectation failed: expected nullptr, got {}",
                                         static_cast<const void*>(ptr)),
                       loc);
  }
}

template <typename Func>
auto expect_contract_violation(Func f,
                               const std::source_location& loc = std::source_location::current()) {
  contract_violation_occurred = false;
  auto check = [&] {
    bool occurred = std::exchange(contract_violation_occurred, false);
    if (!occurred) add_failure_expect("Expected contract violation, but none was detected", loc);
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
auto expect_no_contract_violation(
    Func f, const std::source_location& loc = std::source_location::current()) {
  contract_violation_occurred = false;
  auto check = [&] {
    bool occurred = std::exchange(contract_violation_occurred, false);
    if (occurred) add_failure_expect("Expected no contract violation, but one was detected", loc);
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
void expect_equal(T&& first, U&& second, std::string_view message,
                  std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_not_equal(T&& first, U&& second, std::string_view message,
                      std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_not_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_less(T&& first, U&& second, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_less(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_less_equal(T&& first, U&& second, std::string_view message,
                       std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_less_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_greater(T&& first, U&& second, std::string_view message,
                    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_greater(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_greater_equal(T&& first, U&& second, std::string_view message,
                          std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_greater_equal(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T, typename U>
void expect_contains(T&& first, U&& second, std::string_view message,
                     std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_contains(std::forward<T>(first), std::forward<U>(second), location);
}
template <typename T>
void expect_true(T&& value, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_true(std::forward<T>(value), location);
}
template <typename T>
void expect_false(T&& value, std::string_view message,
                  std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_false(std::forward<T>(value), location);
}
template <typename T>
void expect_null(T&& value, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_null(std::forward<T>(value), location);
}
template <typename T, typename U, typename K>
void expect_near(T expected, U actual, K tolerance, std::string_view message,
                 std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_near(expected, actual, tolerance, location);
}
template <typename E = std::exception, typename F>
void expect_throws(F&& callable, std::string_view message,
                   std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_throws<E>(std::forward<F>(callable), location);
}
template <typename E = std::exception, typename F>
void expect_throws_exact(F&& callable, std::string_view message,
                         std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_throws_exact<E>(std::forward<F>(callable), location);
}
template <typename F>
decltype(auto) expect_contract_violation(
    F&& callable, std::string_view message,
    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  return expect_contract_violation(std::forward<F>(callable), location);
}
template <typename F>
decltype(auto) expect_no_contract_violation(
    F&& callable, std::string_view message,
    std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  return expect_no_contract_violation(std::forward<F>(callable), location);
}
}  // namespace annotest
