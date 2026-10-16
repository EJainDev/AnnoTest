#include <cstdlib>
import annotest;
import std;
using namespace annotest;

bool available() {
  auto mode = std::getenv("ANNOTEST_CONDITION");
  std::string_view value = mode ? mode : "on";
  std::println("condition checked");
  if (value == "throw") throw std::runtime_error("condition exception");
  if (value == "unknown") throw 42;
  if (value == "skip") skip("condition requested skip");
  return value != "off";
}
bool unavailable() { return false; }
bool even_repetition(const test_context& context) { return context.repetition % 2 == 0; }
bool second_parameter(const test_context& context) { return context.name.ends_with(".2"); }
std::vector<named_case<int>> inputs() { return {case_of("one", 1), case_of("two", 2)}; }

struct[[= enabled_if<&available>{"suite unavailable"}]] Guarded {
  Guarded() { std::println("constructed"); }
  ~Guarded() { std::println("destroyed"); }
  [[= before_each{}]] void setup() { std::println("setup"); }
  [[= after_each{}]] void cleanup() { std::println("cleanup"); }
  [[= test{}]] void body() { std::println("body"); }
  struct[[= nested{}]] Child {
    Child(Guarded&) { std::println("child constructed"); }
    [[= test{}]] void body() { std::println("child body"); }
  };
};
struct InheritedGuard : Guarded {};
struct[[= disabled_if<&available>{"suite denied"}]] Denied {
  Denied() { std::println("constructed"); }
  [[= test{}]] void body() { std::println("body"); }
};
struct ExecutionTests {
  int counter = 0;
  ExecutionTests() { std::println("constructed"); }
  ~ExecutionTests() { std::println("destroyed"); }
  [[ = test{}, = enabled_if<&available>{"method unavailable"} ]] void conditional() {
    std::println("body");
  }
  [[ = test{}, = enabled_if<&unavailable>{nullptr} ]] void default_reason() { fail("unreachable"); }
  [[ = test{}, = enabled_if<&unavailable>{"first condition"},
     = enabled_if<&available>{} ]] void short_circuit() {
    fail("unreachable");
  }
  [[ = test{.disabled = true}, = enabled_if<&available>{} ]] void disabled() {
    std::println("body");
  }
  [[= test{.repetitions = 3}]] void repeated() {
    assert_equal(0, counter++);
    record_property("index", std::to_string(current_test().repetition));
    std::println("body");
  }
  [[ = test{.repetitions = 3},
     = enabled_if<&even_repetition>{"odd repetition"} ]] void conditional_repeat() {
    assert_equal(0, counter++);
    record_property("index", std::to_string(current_test().repetition));
  }
  [[
    = test{.repetitions = 2}, = parameterize{parameters(1), parameters(2)},
    = enabled_if<&second_parameter>{"first parameter"}
  ]] void parameterized(int value) {
    assert_equal(2, value);
  }
  [[ = test{.repetitions = 2}, = parameter_source<&inputs>{} ]] void runtime_parameters(int value) {
    assert_equal(0, counter++);
    record_property("value", std::to_string(value));
  }
  template <typename T = int>
  [[
    = test{.repetitions = 2},
    = parameterize_template<template_params<int>, template_params<double>>{}
  ]] void typed() {
    assert_greater(sizeof(T), 0u);
  }
  [[= test{.repetitions = 3}]] void repeated_failure() { expect_true(false); }
  [[ = test{.repetitions = 2}, = test_factory{} ]] auto dynamic() {
    return std::vector<dynamic_case>{{"one", [] { assert_true(true); }},
                                     {"two", [] { assert_true(true); }}};
  }
  [[ = test{}, = enabled_if<&available>{"factory unavailable"},
     = test_factory{} ]] auto conditional_factory() {
    std::println("factory body");
    return std::vector<dynamic_case>{{"one", [] { std::println("child body"); }}};
  }
  [[= test{}]] void group_success() {
    assert_all();
    expect_all("empty");
    assert_all("passing", [] { assert_equal(1, 1); }, [] { expect_true(true); });
    assert_all([] {});
    expect_all([] {});
    // Captured failures inside a successful block do not fail the group.
    assert_all([] {
      auto captured = capture_failures([] { expect_true(false); });
      assert_equal(1u, captured.size());
    });
  }
  [[= test{}]] void group_fatal() {
    assert_all(
        "independent blocks",
        [] {
          assert_true(false);
          std::println("unreachable block");
        },
        [] {
          expect_true(false);
          std::println("second block");
        },
        [] { std::println("third block"); });
    std::println("unreachable after group");
  }
  [[= test{}]] void group_nonfatal() {
    expect_all(
        "independent blocks",
        [] {
          assert_true(false);
          std::println("unreachable block");
        },
        [] {
          expect_true(false);
          std::println("second block");
        });
    std::println("continued after group");
  }
  [[= test{}]] void group_exceptions() {
    expect_all(
        "exceptions", [] { throw std::runtime_error("block exception"); }, [] { throw 42; },
        [] { std::println("last block"); });
    std::println("continued after group");
  }
  [[= test{}]] void group_nested() {
    assert_all(
        "outer",
        [] {
          assert_all("inner", [] { assert_true(false); }, [] { std::println("inner continued"); });
        },
        [] { std::println("outer continued"); });
  }
  [[= test{}]] void group_skip() {
    assert_all("skip", [] { skip("group skip"); }, [] { fail("unreachable"); });
  }
  [[= test{}]] void group_capture() {
    auto captured = capture_failures([] {
      assert_all([] { expect_true(false); }, [] { assert_true(false); });
      fail("unreachable after captured group");
    });
    assert_equal(2u, captured.size());
    assert_false(has_failure());
    std::println("capture restored");
  }
  [[= test{}]] void group_move_only() {
    assert_all([value = std::make_unique<int>(7)] { assert_equal(7, *value); });
  }
  [[= test{}]] void group_thread_capture() {
    auto captured = capture_failures(
        [] {
          auto worker = test_thread(
              [] { assert_all([] { expect_true(false); }, [] { assert_true(false); }); });
          worker.join();
        },
        true);
    assert_equal(2u, captured.size());
    assert_false(has_failure());
  }
};
struct[[= suite{.lifecycle = fixture_lifecycle::shared}]] SharedRepeats {
  int counter = 0;
  [[= test{.repetitions = 3}]] void repeated() {
    assert_equal(static_cast<int>(current_test().repetition), counter++);
  }
};
int main(int argc, char** argv) {
  auto scenario = std::getenv("ANNOTEST_SCENARIO");
  std::string_view mode = scenario ? scenario : "";
  if (mode == "outside-group") {
    assert_all([] {});
    expect_all([] {});
    return 0;
  }
  if (mode == "guarded") return run_tests<Guarded>(argc, argv);
  if (mode == "inherited") return run_tests<InheritedGuard>(argc, argv);
  if (mode == "denied") return run_tests<Denied>(argc, argv);
  if (mode == "shared") return run_tests<SharedRepeats>(argc, argv);
  return run_tests<ExecutionTests>(argc, argv);
}
