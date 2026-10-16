#include <cstdlib>
import annotest;
import std;
using namespace annotest;

std::string_view scenario() {
  auto p = std::getenv("ANNOTEST_SCENARIO");
  return p ? p : "";
}
struct HookBase {
  int value = 0;
  [[= before_each{}]] void base_setup() {
    std::println("base setup");
    value = 1;
  }
  [[= after_each{}]] void base_cleanup() { std::println("base cleanup {}", value); }
  [[= test{}]] void replaced() { fail("base method must be overridden"); }
  [[= test{}]] void hidden() { fail("unannotated override must hide this test"); }
};
struct HookSuite : HookBase {
  [[= before_each{}]] void setup_a() {
    std::println("derived setup a");
    ++value;
  }
  [[= before_each{}]] void setup_b() {
    std::println("derived setup b");
    ++value;
  }
  [[= after_each{}]] void cleanup_a() { std::println("derived cleanup a"); }
  [[= after_each{}]] void cleanup_b() {
    std::println("derived cleanup b");
    if (scenario() == "cleanup") throw std::runtime_error("cleanup b failed");
  }
  [[= test{}]] void replaced() { assert_equal(3, value); }
  void hidden() {}
  [[= test{}]] void body() { assert_equal(3, value); }
};
struct OuterSuite {
  int value = 0;
  OuterSuite() { std::println("outer constructor"); }
  ~OuterSuite() { std::println("outer destructor"); }
  [[= before_each{}]] void setup() {
    value = 10;
    std::println("outer setup");
    if (scenario() == "outer-setup") throw std::runtime_error("outer setup failed");
  }
  [[= after_each{}]] void cleanup() { std::println("outer cleanup {}", value); }
  struct[[= nested{}]] Inner {
    OuterSuite& outer;
    Inner(OuterSuite& outer) : outer(outer) {
      std::println("inner constructor");
      if (scenario() == "inner-constructor") throw std::runtime_error("inner constructor failed");
    }
    ~Inner() { std::println("inner destructor"); }
    [[= before_each{}]] void setup() {
      ++outer.value;
      std::println("inner setup");
    }
    [[= after_each{}]] void cleanup() {
      std::println("inner cleanup");
      --outer.value;
    }
    [[= test{}]] void first() { assert_equal(11, outer.value); }
    [[= test{}]] void second() { assert_equal(11, outer.value); }
    struct[[= nested{}]] Deep {
      Inner& inner;
      Deep(Inner& inner) : inner(inner) {}
      [[= test{}]] void body() { assert_equal(11, inner.outer.value); }
    };
  };
};
template <fixture_lifecycle Lifecycle>
struct[[= suite{.lifecycle = Lifecycle}]] NestedLifetime {
  int value = 10;
  NestedLifetime() { std::println("parent constructor"); }
  ~NestedLifetime() { std::println("parent destructor"); }
  struct[[= nested{}, = suite{.lifecycle = fixture_lifecycle::shared}]] Child {
    NestedLifetime& parent;
    Child(NestedLifetime& parent) : parent(parent) { std::println("child constructor"); }
    ~Child() { std::println("child destructor {}", parent.value); }
    [[= test{}]] void first() { assert_equal(10, parent.value++); }
    [[= test{}]] void second() { assert_equal(11, parent.value++); }
  };
};
struct Resource {
  std::filesystem::path file;
  explicit Resource(std::filesystem::path file) : file(std::move(file)) {}
  ~Resource() { std::ofstream(file) << "closed"; }
};
struct[[= suite{.lifecycle = fixture_lifecycle::shared}]] SharedContext {
  test_context& context;
  SharedContext(test_context& context) : context(context) { context.temp_dir(); }
  ~SharedContext() noexcept(false) {
    std::println("shared destructor {}", context.name);
    if (scenario() == "destructor") throw std::runtime_error("destructor failed");
  }
  [[= before_each{}]] void setup() { assert_equal(context.name, current_test().name); }
  [[= test{}]] void first() { record_property("fixture-directory", context.temp_dir().string()); }
  [[= test{}]] void second() { record_property("fixture-directory", context.temp_dir().string()); }
};
struct DynamicSuite {
  int value = 0;
  [[= before_each{}]] void setup() {
    std::println("factory setup");
    value = 7;
    if (scenario() == "factory-setup") throw std::runtime_error("factory setup failed");
  }
  [[= after_each{}]] void cleanup() {
    std::println("factory cleanup {}", value);
    if (scenario() == "factory-cleanup") throw std::runtime_error("factory cleanup failed");
  }
  [[ = test{}, = test_factory{} ]] auto generated() {
    if (scenario() == "factory-skip") skip("factory unavailable");
    if (scenario() == "empty-factory") return std::vector<dynamic_case>{};
    if (scenario() == "duplicate-factory")
      return std::vector<dynamic_case>{{"same", [] {}}, {"same", [] {}}};
    return std::vector<dynamic_case>{{"first",
                                      [this] {
                                        assert_equal(7, value);
                                        value = 8;
                                      }},
                                     {"second", [this] {
                                        assert_equal(8, value);
                                        value = 9;
                                      }}};
  }
};
struct SimpleSuite {
  [[= test{}]] void passing() { assert_true(true); }
  [[= test{}]] void failure() { expect_equal(1, 2); }
  [[= test{}]] void exception() { throw std::runtime_error("worker exception"); }
  [[= test{}]] void unknown() { throw 42; }
  [[= test{}]] void contract() { contract_violation_occurred = true; }
  [[= test{.disabled = true}]] void disabled() {}
  [[= test{}]] void empty_skip() { skip(""); }
  [[= test{}]] void resources() {
    auto& store = current_test().resources;
    auto first = store.get_or_create<int>("integer", [] { return std::make_shared<int>(7); });
    auto same = store.get_or_create<int>("integer", [] { return std::make_shared<int>(8); });
    assert_equal(first, same);
    assert_equal(7, *same);
    assert_throws<std::logic_error>([&] {
      store.get_or_create<std::string>("integer", [] { return std::make_shared<std::string>(); });
    });
    assert_throws<std::invalid_argument>(
        [&] { store.get_or_create<int>("null", [] { return std::shared_ptr<int>{}; }); });
  }
};
struct Extension : test_listener {
  int number;
  explicit Extension(int number) : number(number) {}
  void before_test() override {
    std::println("extension setup {}", number);
    current_test().resources.get_or_create<int>("extension",
                                                [] { return std::make_shared<int>(1); });
    if (scenario() == "extension-setup" && number == 1)
      throw std::runtime_error("extension setup failed");
  }
  void after_test() override {
    std::println("extension cleanup {}", number);
    if (scenario() == "extension-cleanup" && number == 2)
      throw std::runtime_error("extension cleanup failed");
  }
  void on_assertion(const assertion_event& event) override {
    if (scenario() == "replay" && number == 1) {
      if (!event.location || !event.location->first.ends_with("scope_tests.cpp"))
        throw std::runtime_error("Missing replayed source location");
      std::ofstream(std::getenv("ANNOTEST_EVENT_FILE"), std::ios::app)
          << event.phase << ":" << event.message << "\n";
    }
    if (scenario() == "observer-assertion" && number == 1)
      throw std::runtime_error("assertion observer failed");
  }
  void on_test_end(const test_result&) override {
    if (scenario() == "observer-end" && number == 1)
      throw std::runtime_error("end observer failed");
  }
  void on_suite_start(std::string_view) override {
    if (scenario() == "observer-start" && number == 1) throw 42;
  }
  void on_run_end(std::span<const test_result>) override {
    if (scenario() == "observer-run-end" && number == 1)
      throw std::runtime_error("run observer failed");
  }
};
struct ScopedObserver : test_listener {
  void on_discovery(std::string_view suite, std::string_view) override {
    if (!suite.starts_with("OuterSuite")) throw std::runtime_error("Foreign discovery");
  }
  void on_test_start(std::string_view suite, std::string_view) override {
    if (!suite.starts_with("OuterSuite")) throw std::runtime_error("Foreign start");
  }
  void on_run_end(std::span<const test_result> results) override {
    for (const auto& result : results)
      if (!result.suite.starts_with("OuterSuite")) throw std::runtime_error("Foreign results");
  }
};
struct TeardownObserver : test_listener {
  std::vector<std::string> started, ended;
  void on_test_start(std::string_view, std::string_view name) override {
    started.emplace_back(name);
  }
  void on_test_end(const test_result& result) override { ended.push_back(result.name); }
  void on_run_end(std::span<const test_result> results) override {
    if (started != ended || results.size() != ended.size() ||
        results.back().name != "_fixture_teardown" || results.back().status != test_status::failed)
      throw std::runtime_error("Missing fixture teardown result");
  }
};
int main(int argc, char** argv) {
  auto mode = scenario();
  if (mode == "nested-shared-parent")
    return run_tests<NestedLifetime<fixture_lifecycle::shared>>(argc, argv);
  if (mode == "nested-per-test-parent")
    return run_tests<NestedLifetime<fixture_lifecycle::per_test>>(argc, argv);
  if (mode == "outside") {
    assert_throws<std::logic_error>([] { current_test(); });
    assert_throws<std::logic_error>([] { record_property("key", "value"); });
    assert_throws<std::invalid_argument>([] { add_listener(nullptr); });
    clear_listeners();
    return 0;
  }
  if (mode.starts_with("extension") || mode.starts_with("observer") || mode == "replay") {
    add_listener(std::make_shared<Extension>(1));
    add_listener(std::make_shared<Extension>(2));
    return run_tests<SimpleSuite>(argc, argv);
  }
  if (mode == "scoped") {
    add_listener(std::make_shared<ScopedObserver>(), "OuterSuite");
    return run_tests<SimpleSuite, OuterSuite>(argc, argv);
  }
  if (mode == "shared" || mode == "destructor") {
    if (mode == "destructor") add_listener(std::make_shared<TeardownObserver>());
    return run_tests<SharedContext>(argc, argv);
  }
  if (mode.starts_with("factory") || mode.ends_with("factory"))
    return run_tests<DynamicSuite>(argc, argv);
  if (mode.starts_with("outer") || mode.starts_with("inner") || mode == "nested")
    return run_tests<OuterSuite>(argc, argv);
  if (mode == "hooks" || mode == "cleanup") return run_tests<HookSuite>(argc, argv);
  return run_tests<SimpleSuite>(argc, argv);
}
