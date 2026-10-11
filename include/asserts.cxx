export module annotest:asserts;

import :exceptions;
import :contracts;
import :utils;
import :logger;

import std;

export namespace annotest {
inline void add_failure(const std::string_view msg, const std::source_location& loc) {
  suite_log.add_failure(failure_xml_log{
      failure_types::assertion, std::string{msg},
      failure_cdata{std::make_pair(loc.file_name(), loc.line()), std::string{msg}}});

  throw assertion_error{std::string{msg}, true};
}

// Assert statements that can be called
void assert_equal(auto expected, auto actual,
                  const std::source_location& loc = std::source_location::current()) {
  if (expected != actual) {
    add_failure(std::format("Assertion failed: expected {}, got {}", expected, actual), loc);
  }
}

void assert_not_equal(auto expected, auto actual,
                      const std::source_location& loc = std::source_location::current()) {
  if (expected == actual) {
    add_failure(std::format("Assertion failed: expected not equal to {}, got {}", expected, actual),
                loc);
  }
}

void assert_true(auto value, const std::source_location& loc = std::source_location::current()) {
  if (!value) {
    add_failure(std::format("Assertion failed: expected true, got false for {}", value), loc);
  }
}

void assert_false(auto value, const std::source_location& loc = std::source_location::current()) {
  if (value) {
    add_failure(std::format("Assertion failed: expected false, got true for {}", value), loc);
  }
}

template <typename T, typename U, typename K = std::common_type_t<T, U>>
void assert_near(T expected, U actual, K tol = static_cast<K>(0.001),
                 const std::source_location& loc = std::source_location::current()) {
  if (!(std::abs(expected - actual) <= tol)) {
    add_failure(
        std::format("Assertion failed: expected {}, got {} within {}", expected, actual, tol), loc);
  }
}

void assert_less(auto a, auto b,
                 const std::source_location& loc = std::source_location::current()) {
  if (!(a < b)) {
    add_failure(std::format("Assertion failed: expected {}, got {}", a, b), loc);
  }
}

void assert_less_equal(auto a, auto b,
                       const std::source_location& loc = std::source_location::current()) {
  if (!(a <= b)) {
    add_failure(std::format("Assertion failed: expected {}, got {}", a, b), loc);
  }
}

void assert_greater(auto a, auto b,
                    const std::source_location& loc = std::source_location::current()) {
  if (!(a > b)) {
    add_failure(std::format("Assertion failed: expected {}, got {}", a, b), loc);
  }
}

void assert_greater_equal(auto a, auto b,
                          const std::source_location& loc = std::source_location::current()) {
  if (!(a >= b)) {
    add_failure(std::format("Assertion failed: expected {}, got {}", a, b), loc);
  }
}

void assert_contains(auto const& container, auto const& value,
                     const std::source_location& loc = std::source_location::current()) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    add_failure(std::format("Assertion failed: container does not contain value {}", value), loc);
  }
}

template <typename E = std::exception>
void assert_throws(auto func, const std::source_location& loc = std::source_location::current()) {
  try {
    func();
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
  } catch (const E& e) {
    if (typeid(e) != typeid(E)) {
      add_failure(std::format("Expected exact exception type {}, got {}", typeid(E).name(),
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
    add_failure(
        std::format("Assertion failed: expected nullptr, got {}", static_cast<const void*>(ptr)),
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
}  // namespace annotest
