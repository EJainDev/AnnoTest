export module annotest:expects;

import :exceptions;
import :utils;
import :contracts;

import std;

namespace annotest {
// Expect statements that can be called
export void expectEqual(auto expected, auto actual) {
  if (expected != actual) {
    std::println(std::cerr, "Expectation failed: expected {}, got {}", expected, actual);
  }
}

export void expectNotEqual(auto expected, auto actual) {
  if (expected == actual) {
    std::println(std::cerr, "Expectation failed: expected not equal to {}, but got {}", expected,
                 actual);
  }
}

export void expectTrue(auto value) {
  if (!value) {
    std::println(std::cerr, "Expectation failed: expected true, got false for {}", value);
  }
}

export void expectFalse(auto value) {
  if (value) {
    std::println(std::cerr, "Expectation failed: expected false, got true for {}", value);
  }
}

export template <typename T, typename U, typename K = std::common_type_t<T, U>>
void expectNear(T expected, U actual, K tol = static_cast<K>(1e-3)) {
  if (!(std::abs(expected - actual) <= tol)) {
    std::println(std::cerr, "Expectation failed: expected {}, got {} within {}", expected, actual,
                 tol);
  }
}

export void expectLess(auto a, auto b) {
  if (!(a < b)) {
    std::println(std::cerr, "Expectation failed: expected {}, got {}", a, b);
  }
}

export void expectLessEqual(auto a, auto b) {
  if (!(a <= b)) {
    std::println(std::cerr, "Expectation failed: expected {}, got {}", a, b);
  }
}

export void expectGreater(auto a, auto b) {
  if (!(a > b)) {
    std::println(std::cerr, "Expectation failed: expected {}, got {}", a, b);
  }
}

export void expectGreaterEqual(auto a, auto b) {
  if (!(a >= b)) {
    std::println(std::cerr, "Expectation failed: expected {}, got {}", a, b);
  }
}

export void expectContains(auto const& container, auto const& value) {
  if (std::find(std::begin(container), std::end(container), value) == std::end(container)) {
    std::println(std::cerr, "Expectation failed: container does not contain value {}", value);
  }
}

export template <typename E = std::exception>
void expectThrows(auto func) {
  try {
    func();
    std::println(std::cerr,
                 "Expectation failed: expected exception of type {}, but no exception was thrown",
                 typeid(E).name());
  } catch (const E&) {
    return;
  } catch (...) {
    std::println(
        std::cerr,
        "Expectation failed: expected exception of type {}, but got different exception type",
        typeid(E).name());
  }
}

export template <typename E>
void expectThrowsExact(auto func) {
  try {
    func();
    std::println(std::cerr, "Expectation failed: expected exception, none thrown");
  } catch (...) {
    auto ep = std::current_exception();
    try {
      std::rethrow_exception(ep);
    } catch (const E& e) {
      if (typeid(e) != typeid(E)) {
        std::println(std::cerr, "Expectation failed: expected exact exception type {}, got {}",
                     typeid(E).name(), typeid(e).name());
      }
      return;  // exact match
    } catch (...) {
      std::println(std::cerr,
                   "Expectation failed: expected exception of type {}, but got different type",
                   typeid(E).name());
    }
  }
}

export void expectNull(auto ptr) {
  if (ptr != nullptr) {
    std::println(std::cerr, "Expectation failed: expected nullptr, got {}", ptr);
  }
}

export template <typename Func>
void expectContractViolation(Func f) {
  if constexpr (!std::is_void_v<std::invoke_result_t<Func>>) {
    auto result = f();
    if (!annotest::contract_violation_occurred) {
      std::println(std::cerr,
                   "Expectation failed: expected contract violation, but it was not detected");
    }
    return;
  }
  f();
  if (!annotest::contract_violation_occurred) {
    std::println(std::cerr,
                 "Expectation failed: expected contract violation, but it was not detected");
  }
}

export template <typename Func>
auto expectNoContractViolation(Func f) {
  if constexpr (!std::is_void_v<std::invoke_result_t<Func>>) {
    auto result = f();
    if (annotest::contract_violation_occurred) {
      std::println(std::cerr,
                   "Expectation failed: expected no contract violation, but one was detected");
    }
    return result;
  }
  f();
  if (annotest::contract_violation_occurred) {
    std::println(std::cerr,
                 "Expectation failed: expected no contract violation, but one was detected");
  }
}
}  // namespace annotest