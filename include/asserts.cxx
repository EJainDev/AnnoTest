export module annotest:asserts;

import :exceptions;
import :contracts;
import :utils;
// import :logger;

import std;

export namespace annotest {
// Assert statements that can be called
void assertEqual(auto expected, auto actual) {
  if (expected != actual) {
    throw assertion_error{std::format("Assertion failed: expected {}, got {}", expected, actual)};
  }
}

void assertNotEqual(auto expected, auto actual) {
  if (expected == actual) {
    throw assertion_error{
        std::format("Assertion failed: expected not equal to {}, got {}", expected, actual)};
  }
}

void assertTrue(auto value) {
  if (!value) {
    throw assertion_error{std::format("Assertion failed: expected true, got false for {}", value)};
  }
}

void assertFalse(auto value) {
  if (value) {
    throw assertion_error{std::format("Assertion failed: expected false, got true for {}", value)};
  }
}

template <typename T, typename U, typename K = std::common_type_t<T, U>>
void assertNear(T expected, U actual, K tol = static_cast<K>(0.001)) {
  if (!(std::abs(expected - actual) <= tol)) {
    throw assertion_error{
        std::format("Assertion failed: expected {}, got {} within {}", expected, actual, tol)};
  }
}

void assertLess(auto a, auto b) {
  if (!(a < b)) {
    throw assertion_error{std::format("Assertion failed: expected {}, got {}", a, b)};
  }
}

void assertLessEqual(auto a, auto b) {
  if (!(a <= b)) {
    throw assertion_error{std::format("Assertion failed: expected {}, got {}", a, b)};
  }
}

void assertGreater(auto a, auto b) {
  if (!(a > b)) {
    throw assertion_error{std::format("Assertion failed: expected {}, got {}", a, b)};
  }
}

void assertGreaterEqual(auto a, auto b) {
  if (!(a >= b)) {
    throw assertion_error{std::format("Assertion failed: expected {}, got {}", a, b)};
  }
}

void assertContains(auto const& container, auto const& value) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    throw assertion_error{
        std::format("Assertion failed: container does not contain value {}", value)};
  }
}

template <typename E = std::exception>
void assertThrows(auto func) {
  try {
    func();
    throw assertion_error{std::format("Assertion failed: expected exception, none thrown")};
  } catch (const E&) {
    return;
  } catch (...) {
    throw assertion_error{
        std::format("Assertion failed: thrown exception type did not match expected")};
  }
}

template <typename E, typename... Args>
void assertThrowsExact(auto f, Args&&... args)
  requires(std::is_invocable_v<decltype(f), decltype(args)...>)
{
  try {
    f(std::forward<Args>(args)...);
    throw assertion_error{std::format("Assertion failed: expected exception, none thrown")};
  } catch (...) {
    auto ep = std::current_exception();
    try {
      std::rethrow_exception(ep);
    } catch (const E& e) {
      if (typeid(e) != typeid(E)) {
        throw assertion_error{
            std::format("Assertion failed: expected exact exception type {}, got {}",
                        typeid(E).name(), typeid(e).name())};
      }
      return;  // exact match
    } catch (...) {
      throw assertion_error{
          std::format("Assertion failed: thrown exception type did not match expected")};
    }
  }
}

void assertNull(auto ptr) {
  if (ptr != nullptr) {
    throw assertion_error{std::format("Assertion failed: expected nullptr, got {}", ptr)};
  }
}

template <typename Func, typename... Args>
auto assertContractViolation(Func f, Args&&... args)
  requires(std::is_invocable_v<Func, Args...>)
{
  if constexpr (!std::is_void_v<std::invoke_result_t<Func>>) {
    auto result = f(std::forward<Args>(args)...);
    if (!annotest::contract_violation_occurred) {
      throw assertion_error{
          std::format("Assertion failed: expected contract violation, but it was not detected")};
    }
    return result;
  }
  f(std::forward<Args>(args)...);
  if (!annotest::contract_violation_occurred) {
    throw assertion_error{
        std::format("Assertion failed: expected contract violation, but it was not detected")};
  }
}

template <typename Func, typename... Args>
auto assertNoContractViolation(Func f, Args&&... args)
  requires(std::is_invocable_v<Func, Args...>)
{
  if constexpr (!std::is_same_v<decltype(f()), void>) {
    auto result = f(std::forward<Args>(args)...);

    if (annotest::contract_violation_occurred) {
      throw assertion_error{
          std::format("Assertion failed: expected no contract violation, but one was detected")};
    }

    return result;
  }

  f(std::forward<Args>(args)...);

  if (annotest::contract_violation_occurred) {
    throw assertion_error{
        std::format("Assertion failed: expected no contract violation, but one was detected")};
  }
}
}  // namespace annotest