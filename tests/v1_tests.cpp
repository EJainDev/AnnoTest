#include <cstdlib>
import annotest;
import std;
using namespace annotest;

std::string_view mode() {
  auto value = std::getenv("ANNOTEST_SCENARIO");
  return value ? value : "basic";
}
std::vector<named_case<int>> v1_inputs() { return {case_of("small", 1), case_of("large", 2)}; }
struct suite_base {
  inline static int setups = 0;
  [[= before_all{}]] static void suite_setup(test_context& context) {
    ++setups;
    record_property("suite_resource", "shared");
    std::println("base before_all");
    assert_equal("_suite", context.name);
    auto resource =
        context.resources.get_or_create<int>("value", [] { return std::make_shared<int>(7); });
    assert_equal(7, *resource);
    std::ofstream(context.temp_dir() / "marker") << "suite resource";
  }
  [[= after_all{}]] static void suite_cleanup(test_context& context) {
    assert_equal(1, setups);
    assert_true(std::filesystem::exists(context.temp_dir() / "marker"));
    assert_equal(
        7, *context.resources.get_or_create<int>("value", [] { return std::make_shared<int>(0); }));
    std::println("base after_all");
  }
};
struct[[ = suite<"v1">{}, = tag<"v1">{} ]] fresh_suite : suite_base {
  int value = 0;
  fresh_suite() { std::println("constructed"); }
  ~fresh_suite() { std::println("destroyed"); }
  [[= before_all{}]] static void setup() { std::println("derived before_all"); }
  [[= after_all{}]] static void cleanup() { std::println("derived after_all"); }
  [[= before_each{}]] void before() { assert_equal(0, value++); }
  [[= after_each{}]] void after() { assert_equal(1, value); }
  [[ = test{.repetitions = 2}, = parameter_source<&v1_inputs>{},
     = tag<"data">{} ]] void data(int input) {
    assert_equal(1, setups);
    assert_that(input, any_of(eq(1), eq(2)));
    std::println("body {}", input);
  }
  [[ = test{}, = parameterize{parameters(3), parameters(4)} ]] void values(int input) {
    assert_greater(input, 0);
  }
  template <typename t = int>
  [[ = test{}, = parameterize_template<template_params<int>, template_params<double>>{} ]] void
      types() {
    assert_greater(sizeof(t), 0u);
  }
  [[= test{.disabled = true}]] void disabled() { fail("disabled body"); }
};
struct[[= suite<"shared">{.lifecycle = fixture_lifecycle::shared}]] shared_suite : suite_base {
  int counter = 0;
  [[= test{.repetitions = 2}]] void first() {
    record_property("counter", std::to_string(++counter));
    assert_equal(1, setups);
  }
  [[= test{}]] void second() { record_property("counter", std::to_string(++counter)); }
};
struct[[= suite<"outer">{}]] outer_suite {
  [[= before_all{}]] static void setup() { std::println("outer before_all"); }
  [[= after_all{}]] static void cleanup() {
    std::println("outer after_all");
    if (mode() == "nested-cleanup") throw std::runtime_error("outer cleanup failure");
  }
  [[= before_each{}]] void before() { std::println("outer before_each"); }
  [[= after_each{}]] void after() { std::println("outer after_each"); }
  [[= test{}]] void own() { std::println("outer body"); }
  struct[[ = nested{}, = suite<"inner">{} ]] inner_suite {
    inner_suite(outer_suite&) { std::println("inner constructed"); }
    [[= before_all{}]] static void setup() { std::println("inner before_all"); }
    [[= after_all{}]] static void cleanup() { std::println("inner after_all"); }
    [[= test{.repetitions = 2}]] void body() { std::println("inner body"); }
  };
};
struct[[= suite<"shared_outer">{.lifecycle = fixture_lifecycle::shared}]] shared_outer {
  int counter = 0;
  [[= before_all{}]] static void start() { std::println("shared outer setup"); }
  [[= after_all{}]] static void finish() { std::println("shared outer cleanup"); }
  [[= before_each{}]] void setup() { ++counter; }
  struct[[ = nested{}, = suite<"inner">{.lifecycle = fixture_lifecycle::shared} ]] child {
    shared_outer& parent;
    int counter = 0;
    explicit child(shared_outer& value) : parent(value) {}
    [[= before_all{}]] static void start() { std::println("shared inner setup"); }
    [[= after_all{}]] static void finish() { std::println("shared inner cleanup"); }
    [[= test{.repetitions = 2}]] void body() {
      assert_equal(parent.counter, ++counter);
      record_property("counter", std::to_string(counter));
    }
  };
};
struct[[= suite<"failures">{}]] failure_suite {
  [[= before_all{}]] static void setup() {
    std::println("suite setup attempted");
    if (mode() == "fatal") assert_true(false, "suite setup fatal");
    if (mode() == "nonfatal") expect_true(false, "suite setup nonfatal");
    if (mode() == "skip") skip("suite unavailable");
    if (mode() == "throw") throw std::runtime_error("suite setup exception");
  }
  [[= after_all{}]] static void cleanup() {
    std::println("suite cleanup attempted");
    if (mode() == "cleanup") throw std::runtime_error("suite cleanup exception");
  }
  [[= after_all{}]] static void later_cleanup() { std::println("later cleanup attempted"); }
  [[= test{}]] void first() { std::println("body executed"); }
  [[= test{}]] void second() { std::println("body executed"); }
};
struct[[= suite<"factory">{}]] factory_suite : suite_base {
  [[ = test{}, = test_factory{}, = tag<"dynamic">{} ]] auto cases() {
    std::println("factory built");
    return std::vector<dynamic_case>{{"one", [] { assert_equal(1, setups); }},
                                     {"two", [] { assert_equal(1, setups); }}};
  }
};
struct[[= suite<"threads">{}]] threaded_suite {
  [[= test{.repetitions = 2}]] void failures() {
    scoped_trace trace("main trace");
    auto worker = test_thread([] {
      scoped_trace thread_trace("thread trace");
      expect_equal(1, 2, "thread failure");
    });
    expect_equal(3, 4, "main failure");
  }
};
struct trace_listener : test_listener {
  std::unique_ptr<scoped_trace> trace;
  void before_test() override { trace = std::make_unique<scoped_trace>("extension trace"); }
  void after_test() override { trace.reset(); }
};
struct[[= suite<"trace_suite">{}]] traced_suite {
  [[= before_all{}]] static void start() { scoped_trace trace("suite-only trace"); }
  [[= after_all{}]] static void finish() {}
  [[= test{}]] void first() { expect_equal(1, 2, "body failure"); }
  [[= test{}]] void second() { expect_equal(3, 4, "later failure"); }
};
struct assertion_listener : test_listener {
  std::vector<std::string> phases;
  void on_assertion(const assertion_event& event) override { phases.push_back(event.phase); }
  void on_run_end(std::span<const test_result> results) override {
    std::size_t failures = 0;
    for (const auto& result : results) failures += result.failures;
    if (failures != phases.size()) throw std::runtime_error("listener lost assertion events");
    std::println("listener assertions {}", phases.size());
  }
};
int main(int argc, char** argv) {
  if (mode() == "nested" || mode() == "nested-cleanup") return run_tests<outer_suite>(argc, argv);
  if (mode() == "shared-nested") return run_tests<shared_outer>(argc, argv);
  if (mode() == "shared") return run_tests<shared_suite>(argc, argv);
  if (mode() == "factory") return run_tests<factory_suite>(argc, argv);
  if (mode() == "traces") {
    add_listener(std::make_shared<trace_listener>());
    return run_tests<traced_suite>(argc, argv);
  }
  if (mode() == "registered") {
    for (std::string name : {"one", "two"})
      register_test<fresh_suite>(
          "registered", name, [] { return std::make_unique<fresh_suite>(); },
          [](fresh_suite& fixture) { assert_equal(1, fixture.value); });
    return run_registered_tests(argc, argv);
  }
  if (mode() == "threads") {
    add_listener(std::make_shared<assertion_listener>());
    return run_tests<threaded_suite>(argc, argv);
  }
  if (mode() != "basic") return run_tests<failure_suite>(argc, argv);
  return run_tests<fresh_suite>(argc, argv);
}
