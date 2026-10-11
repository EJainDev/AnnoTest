export module annotest:expects;

import :contracts;
import :utils;
import :logger;

import std;

export namespace annotest {
inline void add_failure_expect(const std::string_view msg, const std::source_location& loc) {
  suite_log.add_failure(failure_xml_log{
      failure_types::expectation, std::string{msg},
      failure_cdata{std::make_pair(loc.file_name(), loc.line()), std::string{msg}}});
}

// Assert statements that can be called
void expect_equal(auto expected, auto actual,
                  const std::source_location& loc = std::source_location::current()) {
  if (expected != actual) {
    add_failure_expect(std::format("Expectation failed: expected {}, got {}", expected, actual),
                       loc);
  }
}

void expect_not_equal(auto expected, auto actual,
                      const std::source_location& loc = std::source_location::current()) {
  if (expected == actual) {
    add_failure_expect(
        std::format("Expectation failed: expected not equal to {}, got {}", expected, actual), loc);
  }
}

void expect_true(auto value, const std::source_location& loc = std::source_location::current()) {
  if (!value) {
    add_failure_expect(std::format("Expectation failed: expected true, got false for {}", value),
                       loc);
  }
}

void expect_false(auto value, const std::source_location& loc = std::source_location::current()) {
  if (value) {
    add_failure_expect(std::format("Expectation failed: expected false, got true for {}", value),
                       loc);
  }
}

template <typename T, typename U, typename K = std::common_type_t<T, U>>
void expect_near(T expected, U actual, K tol = static_cast<K>(0.001),
                 const std::source_location& loc = std::source_location::current()) {
  if (!(std::abs(expected - actual) <= tol)) {
    add_failure_expect(
        std::format("Expectation failed: expected {}, got {} within {}", expected, actual, tol),
        loc);
  }
}

void expect_less(auto a, auto b,
                 const std::source_location& loc = std::source_location::current()) {
  if (!(a < b)) {
    add_failure_expect(std::format("Expectation failed: expected {}, got {}", a, b), loc);
  }
}

void expect_less_equal(auto a, auto b,
                       const std::source_location& loc = std::source_location::current()) {
  if (!(a <= b)) {
    add_failure_expect(std::format("Expectation failed: expected {}, got {}", a, b), loc);
  }
}

void expect_greater(auto a, auto b,
                    const std::source_location& loc = std::source_location::current()) {
  if (!(a > b)) {
    add_failure_expect(std::format("Expectation failed: expected {}, got {}", a, b), loc);
  }
}

void expect_greater_equal(auto a, auto b,
                          const std::source_location& loc = std::source_location::current()) {
  if (!(a >= b)) {
    add_failure_expect(std::format("Expectation failed: expected {}, got {}", a, b), loc);
  }
}

void expect_contains(auto const& container, auto const& value,
                     const std::source_location& loc = std::source_location::current()) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    add_failure_expect(
        std::format("Expectation failed: container does not contain value {}", value), loc);
  }
}

template <typename E = std::exception>
void expect_throws(auto func, const std::source_location& loc = std::source_location::current()) {
  try {
    func();
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
  } catch (const E& e) {
    if (typeid(e) != typeid(E)) {
      add_failure_expect(std::format("Expected exact exception type {}, got {}", typeid(E).name(),
                                     typeid(e).name()),
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
    add_failure_expect(
        std::format("Expectation failed: expected nullptr, got {}", static_cast<const void*>(ptr)),
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
}  // namespace annotest
