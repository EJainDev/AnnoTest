#include <csignal>
#include <cstdio>
#include <cstdlib>
import annotest;
import std;
using namespace annotest;
#ifndef _WIN32
volatile std::sig_atomic_t traps = 0;
void trap_handler(int) { traps = 1; }
#endif

constexpr std::string prefixed(std::string_view name) { return "case: " + readable_name(name); }
constexpr std::string class_name(std::string_view name) { return "group: " + std::string(name); }
struct[[ = name_generator<&readable_name>{}, = suite_name_generator<&class_name>{} ]] NamedSuite {
  [[= test{}]] void underscores_become_spaces() { assert_true(true); }
  [[ = test<"custom_name">{}, = name_generator<&prefixed>{} ]] void override_name() {}
};
struct[[= parameterize{parameters(2, 3), parameters(10, 20)}]] ConstructorCases {
  int a, b;
  ConstructorCases(int a, int b) : a(a), b(b) {}
  [[= test{}]] void positive() { assert_greater(a + b, 0); }
  [[= test{}]] void context(test_context& context) {
    assert_that(context.suite, starts_with("ConstructorCases."));
  }
  struct[[= nested{}]] Nested {
    ConstructorCases& parent;
    Nested(ConstructorCases& parent) : parent(parent) {}
    [[= test{}]] void value() { assert_greater(parent.b, parent.a); }
  };
};
std::vector<named_case<int>> inputs() {
  auto fault = std::getenv("ANNOTEST_SOURCE_FAULT");
  if (fault && std::string_view(fault) == "empty") return {};
  if (fault && std::string_view(fault) == "duplicate")
    return {case_of("same", 1), case_of("same", 2)};
  if (fault && std::string_view(fault) == "invalid") return {case_of("", 1)};
  return {case_of("small", 1), case_of("large", 10)};
}
struct[[= parameter_source<&inputs>{}]] RuntimeConstructorCases {
  int value;
  test_context& context;
  RuntimeConstructorCases(int value, test_context& context) : value(value), context(context) {}
  [[= test{}]] void positive() {
    assert_greater(value, 0);
    assert_equal(context.name, current_test().name);
  }
};
template <typename T>
struct TypedSuite {
  [[= test{}]] void size() { assert_greater(sizeof(T), 0u); }
};
template <typename T = int>
struct[[= parameterize_template<template_params<int>, template_params<double>>{}]]
    AnnotatedTypedSuite {
  [[= test{}]] void size() { assert_greater(sizeof(T), 0u); }
};
struct FactoryFixture {
  int value;
  explicit FactoryFixture(int value) : value(value) {}
  [[= before_each{}]] void setup() { ++value; }
  [[= after_each{}]] void cleanup() { record_property("cleaned", "yes"); }
};
struct ResultObserver : test_listener {
  std::mutex mutex;
  int assertions = 0;
  void on_assertion(const assertion_event&) override {
    std::lock_guard lock(mutex);
    ++assertions;
  }
  void on_test_discovered(const test_info& info) override {
    if (info.file.empty() || !info.line)
      throw std::runtime_error("Missing discovery source metadata");
  }
  void on_suite_result(const suite_result& result) override {
    if (std::getenv("ANNOTEST_SUITE_OBSERVER_FAILURE"))
      throw std::runtime_error("suite observer failed");
    if (result.failed > result.tests || result.skipped > result.tests)
      throw std::runtime_error("Invalid suite totals");
  }
  void on_test_end(const test_result& result) override {
    if (result.name == "thread_failures") {
      if (result.details.size() != 40 || result.failures != 40 || assertions != 40)
        throw std::runtime_error("Lost helper-thread failure");
    }
    if (result.name == "details") {
      if (result.details.size() != 1 || result.details[0].message.empty() ||
          !result.details[0].location ||
          result.standard_output.find("result output") == std::string::npos ||
          std::ranges::find(result.properties,
                            std::pair{std::string("key"), std::string("value")}) ==
              result.properties.end())
        throw std::runtime_error("Missing detailed result");
    }
  }
};
struct FeatureTests {
  [[= test{}]] void all_thread_capture() {
    auto failures = capture_failures(
        [] {
          std::vector<std::thread> workers;
          for (int i = 0; i < 4; ++i) workers.emplace_back([] { expect_true(false); });
          for (auto& worker : workers) worker.join();
          assert_throws<std::logic_error>([] { capture_failures([] {}, true); });
        },
        true);
    assert_equal(4u, failures.size());
    assert_false(has_failure());
  }
  [[= test{}]] void managed_threads() {
    {
      auto worker = test_thread([] { assert_true(false); });
    }
    {
      auto worker = test_thread([] { throw std::runtime_error("thread exception"); });
    }
    {
      auto worker = test_thread([] { throw 42; });
    }
    {
      auto worker = test_thread([] { assert_true(true); });
    }
    assert_equal(3u, current_failures().size());
  }
  [[= test{}]] void failure_interception() {
    assert_false(has_failure());
    auto failures = capture_failures([] {
      expect_equal(1, 2);
      assert_true(false);
    });
    assert_equal(2u, failures.size());
    assert_equal(failure_types::assertion, failures[1].type);
    assert_false(has_failure());
    expect_failure([] { fail("fatal marker"); }, failure_types::assertion, "fatal marker");
    assert_failure([] { add_failure_message("nonfatal marker"); }, failure_types::expectation,
                   "marker");
    auto mismatch = capture_failures([] {
      expect_failure([] { expect_true(false); }, failure_types::expectation, "does not match");
    });
    assert_equal(1u, mismatch.size());
    assert_throws<std::runtime_error>(
        [] { expect_no_fatal_failure([] { throw std::runtime_error("x"); }); });
    assert_throws<std::runtime_error>(
        [] { assert_no_fatal_failure([] { throw std::runtime_error("x"); }); });
    expect_no_fatal_failure([] {});
    assert_no_fatal_failure([] {});
    assert_throws<std::runtime_error>(
        [] { capture_failures([] { throw std::runtime_error("propagate"); }); });
    auto nested = capture_failures([] {
      auto inner = capture_failures([] { expect_true(false); });
      assert_equal(1u, inner.size());
      expect_false(true);
    });
    assert_equal(1u, nested.size());
  }
  [[= test{}]] void interception_failure() {
    expect_failure([] {}, failure_types::expectation);
    assert_failure([] { expect_true(false); }, failure_types::assertion);
  }
  [[= test{}]] void interception_boundaries() {
    std::function<void()> callable = [] { add_failure_message("expected marker"); };
    expect_failure(callable, failure_types::expectation, "marker");
    assert_failure(callable, failure_types::expectation, "marker");
    auto wrong_type =
        capture_failures([&] { expect_failure(callable, failure_types::assertion, "marker"); });
    assert_equal(1u, wrong_type.size());
    assert_equal(failure_types::expectation, wrong_type[0].type);
    auto wrong_message =
        capture_failures([&] { assert_failure(callable, failure_types::expectation, "missing"); });
    assert_equal(1u, wrong_message.size());
    assert_equal(failure_types::assertion, wrong_message[0].type);
    assert_false(has_failure());
    assert_no_fatal_failure(callable);
    expect_no_fatal_failure(callable);
    assert_equal(2u, current_failures().size());
    assert_true(has_nonfatal_failure());
    assert_false(has_fatal_failure());
  }
  [[= test{}]] void contextual_no_throw_failures() {
    std::function<void()> callable = [] {};
    assert_no_throw(callable, "success context");
    expect_no_throw(callable, "success context");
    callable = [] { throw std::runtime_error("unexpected"); };
    auto failures = capture_failures([&] {
      expect_no_throw(callable, "nonfatal context");
      assert_no_throw(callable, "fatal context");
    });
    assert_equal(2u, failures.size());
    assert_equal(failure_types::expectation, failures[0].type);
    assert_equal(failure_types::assertion, failures[1].type);
    assert_that(failures[0].message, has_substr("nonfatal context"));
    assert_that(failures[1].message, has_substr("fatal context"));
    assert_false(has_failure());
    callable = [] {};
    assert_no_fatal_failure(callable);
    expect_no_fatal_failure(callable);
    callable = [] { fail("original fatal failure"); };
    failures = capture_failures([&] { expect_no_fatal_failure(callable); });
    assert_equal(2u, failures.size());
    assert_equal(failure_types::assertion, failures[0].type);
    assert_equal(failure_types::expectation, failures[1].type);
    failures = capture_failures([&] { assert_no_fatal_failure(callable); });
    assert_equal(2u, failures.size());
    assert_equal(failure_types::assertion, failures[0].type);
    assert_equal(failure_types::assertion, failures[1].type);
    assert_that(failures[0].message, has_substr("original fatal failure"));
    assert_that(failures[1].message, has_substr("Unexpected fatal assertion failure"));
    assert_false(has_failure());
  }
  [[= test{}]] void no_fatal_failure() {
    expect_no_fatal_failure([] { fail("original"); });
    assert_true(has_fatal_failure());
    assert_true(has_nonfatal_failure());
    assert_no_fatal_failure([] { fail("second"); });
  }
  [[= test{}]] void messages() {
    expect_equal(1, 2, "equal message");
    expect_not_equal(1, 1, "not equal message");
    expect_true(false, "true message");
    expect_false(true, "false message");
    expect_less(2, 1, "less message");
    expect_less_equal(2, 1, "less equal message");
    expect_greater(1, 2, "greater message");
    expect_greater_equal(1, 2, "greater equal message");
    expect_contains(std::vector{1}, 2, "contains message");
    expect_near(1.0, 2.0, 0.1, "near message");
    expect_throws<std::runtime_error>([] {}, "throws message");
    expect_throws_exact<std::runtime_error>([] {}, "exact message");
    int value = 1;
    expect_null(&value, "null message");
    assert_null(nullptr, "null success");
    assert_no_throw([] {}, "no throw success");
    expect_no_throw([] {}, "no throw success");
    assert_equal(1, 1, "success");
    assert_not_equal(1, 2, "success");
    assert_true(true, "success");
    assert_false(false, "success");
    assert_less(1, 2, "success");
    assert_less_equal(1, 1, "success");
    assert_greater(2, 1, "success");
    assert_greater_equal(1, 1, "success");
    assert_contains(std::vector{1}, 1, "success");
    assert_near(1.0, 1.0, 0.1, "success");
    assert_throws<std::runtime_error>([] { throw std::runtime_error("x"); }, "success");
    assert_throws_exact<std::runtime_error>([] { throw std::runtime_error("x"); }, "success");
  }
  [[= test{}]] void thread_failures() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i)
      workers.emplace_back([i] {
        scoped_trace trace("worker " + std::to_string(i));
        for (int j = 0; j < 10; ++j) expect_equal(i, -1);
      });
    for (auto& worker : workers) worker.join();
    assert_equal(40u, current_failures().size());
    assert_true(has_nonfatal_failure());
    assert_false(has_fatal_failure());
  }
  [[= test{}]] void details() {
    record_property("key", "value");
    std::println("result output");
    expect_equal(1, 2);
  }
  [[= test{}]] void exit_checks() {
    assert_exit(
        [] {
          std::fprintf(stderr, "goodbye\n");
          std::_Exit(7);
        },
        exited_with_code(7), "goodbye");
    expect_exit([] {}, exited_with_code(0));
    assert_death(
        [] {
          std::fprintf(stderr, "abort marker\n");
          std::abort();
        },
        "abort marker");
    expect_death([] { throw std::runtime_error("still death"); }, ".*");
    expect_death([] { std::abort(); }, ".*");
    expect_death([] { std::_Exit(7); }, ".*");
    assert_death([] { throw std::runtime_error("still death"); }, ".*");
    assert_death([] { std::_Exit(7); }, ".*");
#ifndef _WIN32
    auto mismatch =
        capture_failures([] { expect_exit([] { std::raise(SIGTERM); }, exited_with_code(0)); });
    assert_equal(1u, mismatch.size());
#endif
#ifndef _WIN32
    assert_exit([] { std::raise(SIGTERM); }, killed_by_signal(SIGTERM));
#endif
    assert_throws<std::regex_error>([] { assert_exit([] {}, exited_with_code(0), "["); });
  }
  [[= test{}]] void exit_mismatch() {
    expect_death([] {}, ".*");
    expect_exit([] { std::_Exit(4); }, exited_with_code(7));
    expect_exit(
        [] {
          std::fprintf(stderr, "actual\n");
          std::_Exit(7);
        },
        exited_with_code(7), "expected");
    assert_death([] {}, "anything");
  }
  [[= test{}]] void breakpoint_handled() {
#ifndef _WIN32
    auto previous = std::signal(SIGTRAP, trap_handler);
    expect_true(false);
    std::signal(SIGTRAP, previous);
    assert_equal(1, static_cast<int>(traps));
#endif
  }
  [[= test{}]] void json_bytes() {
    std::fprintf(stdout, "quote\" slash\\ newline\n");
    std::fputc(0xff, stdout);
    record_property("escaped\"key", "value\n");
  }
  [[= test{}]] void passing() {}
  [[= test{}]] void skip_case() { skip("skip reason"); }
};
int main(int argc, char** argv) {
#ifndef _WIN32
  if (std::getenv("ANNOTEST_HANDLE_BREAK")) std::signal(SIGTRAP, trap_handler);
#endif
  auto scenario = std::getenv("ANNOTEST_SCENARIO");
  std::string_view mode = scenario ? scenario : "";
  if (mode == "names") return run_tests<NamedSuite>(argc, argv);
  if (mode == "constructors") return run_tests<ConstructorCases>(argc, argv);
  if (mode == "runtime-constructors") return run_tests<RuntimeConstructorCases>(argc, argv);
  if (mode == "annotated-typed") return run_tests<AnnotatedTypedSuite<>>(argc, argv);
  if (mode == "typed") return run_tests<TypedSuite<int>, TypedSuite<double>>(argc, argv);
  if (mode == "registered-move-only") {
    register_test("Registered", "plain",
                  [value = std::make_unique<int>(3)] { assert_equal(3, *value); });
    register_test<FactoryFixture>(
        "Registered", "factory",
        [value = std::make_unique<int>(7)]() mutable {
          return std::make_unique<FactoryFixture>(*value);
        },
        [expected = std::make_unique<int>(8)](FactoryFixture& fixture) {
          assert_equal(*expected, fixture.value);
        });
    return run_registered_tests(argc, argv);
  }
  if (mode == "registered-empty") return run_registered_tests(argc, argv);
  if (mode == "registered-duplicates") {
    register_test("Registered", "same", [] {});
    register_test("Registered", "same", [] {});
    return run_registered_tests(argc, argv);
  }
  if (mode == "registered-combined") {
    register_test("FeatureTests", "runtime", [] {});
    return run_tests<FeatureTests>(argc, argv);
  }
  if (mode == "registered-context") {
    register_test<FactoryFixture>(
        "Registered", "context",
        [](test_context& context) {
          assert_equal("context", context.name);
          return std::make_unique<FactoryFixture>(7);
        },
        [](FactoryFixture& fixture) { assert_equal(8, fixture.value); });
    return run_registered_tests(argc, argv);
  }
  if (mode == "registered-null") {
    register_test<FactoryFixture>(
        "Registered", "null", [] { return std::unique_ptr<FactoryFixture>{}; },
        [](FactoryFixture&) { fail("must not run"); });
    return run_registered_tests(argc, argv);
  }
  if (mode == "registered") {
    clear_registered_tests();
    register_test("Registered", "plain", [] { assert_equal(2, 1 + 1); }, {"runtime"});
    register_test<FactoryFixture>(
        "Registered", "factory", [] { return std::make_unique<FactoryFixture>(7); },
        [](FactoryFixture& fixture) { assert_equal(8, fixture.value); });
    return run_registered_tests(argc, argv);
  }
  add_listener(std::make_shared<ResultObserver>());
  auto result = run_tests<FeatureTests>(argc, argv);
#ifndef _WIN32
  bool worker = false;
  for (int i = 1; i < argc; ++i) worker |= std::string_view(argv[i]).starts_with("--worker-result");
  if (std::getenv("ANNOTEST_HANDLE_BREAK") && !worker && traps != 1) return 2;
#endif
  return result;
}
