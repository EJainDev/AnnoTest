#include <csignal>
#include <cstdlib>

import annotest;
import std;
using namespace annotest;

std::vector<named_case<int, int>> numbers() {
  auto fault = std::getenv("ANNOTEST_PROVIDER_FAULT");
  if (fault && std::string_view(fault) == "empty") return {};
  if (fault && std::string_view(fault) == "duplicate")
    return {case_of("same", 1, 2), case_of("same", 10, 20)};
  if (fault && std::string_view(fault) == "name-empty") return {case_of("", 1, 2)};
  if (fault && std::string_view(fault) == "name-newline") return {case_of("bad\nname", 1, 2)};
  return {case_of("small", 1, 2), case_of("large", 10, 20)};
}
std::vector<dynamic_case> generated() {
  return {{"one", [] { assert_true(true); }}, {"two", [] { assert_equal(2, 1 + 1); }}};
}
struct StreamOnly {
  int value;
  friend std::ostream& operator<<(std::ostream& stream, const StreamOnly& value) {
    return stream << "stream:" << value.value;
  }
};
struct Unprintable {
  int value;
};
struct[[ = test{}, = tag<"fast">{} ]] Fast {};
struct Base {
  [[= test{}]] void inherited() { assert_true(true); }
};
struct[[= tag<"unit">{}]] ParityTests : Base {
  int counter = 0;
  [[= before_each{}]] void setup() { ++counter; }
  [[= after_each{}]] void teardown() {
    record_property("cleanup", "yes");
    if (current_test().name == "cleanup_failure_after_skip") expect_true(false);
  }
  [[= Fast{}]] void fresh_a() { assert_equal(1, counter); }
  [[= test{}]] void fresh_b() { assert_equal(1, counter); }
  [[ = test{}, = parameterize{parameters(1), parameters(2)} ]] void fresh_values(int) {
    assert_equal(1, counter);
  }
  [[ = test{}, = parameter_source<&numbers>{} ]] void runtime_values(int a, int b) {
    assert_equal(2 * a, b);
    assert_equal(1, counter);
  }
  [[ = test{}, = test_factory<&generated>{} ]] void generated_cases() {}
  [[= test{}]] void context(test_context& ctx) {
    assert_equal("ParityTests", ctx.suite);
    assert_equal("context", ctx.name);
    auto path = ctx.temp_dir() / "data.txt";
    std::ofstream(path) << "hello";
    assert_true(std::filesystem::exists(path));
    record_property("temp_dir", ctx.temp_dir().string());
  }
  [[= test{}]] void runtime_skip() {
    skip("GPU unavailable");
    fail("unreachable");
  }
  [[= test{}]] void assumption() { assume(false, "network unavailable"); }
  [[= test{}]] void matcher_examples() {
    assert_that(std::vector{1, 2, 3}, elements_are(eq(1), gt(1), eq(3)));
    assert_that(std::vector{1, 2}, unordered_elements_are(gt(0), eq(1)));
    assert_that(std::vector{1, 2, 3}, all_of(size_is(eq(3)), each(gt(0)), contains(eq(2))));
    assert_that("abcdef", all_of(starts_with("ab"), ends_with("ef"), has_substr("cd"),
                                 matches_regex("[a-z]+")));
    assert_that(std::optional{3}, optional_value(eq(3)));
    assert_that(std::variant<int, std::string>{3}, variant_with<int>(eq(3)));
    assert_that(std::nextafter(1.0, 2.0), ulp_near(1.0));
    assert_that(1.00001, near(1.0, 0.001));
    assert_that([] { throw std::runtime_error("bad input"); },
                throws<std::runtime_error>(property(&std::runtime_error::what, has_substr("bad"))));
    struct Unformatted {
      int value;
      bool operator==(const Unformatted&) const = default;
    };
    assert_equal(Unformatted{1}, Unformatted{1});
  }
  [[= test{}]] void sources() {
    auto path = current_test().temp_dir() / "cases.csv";
    std::ofstream(path) << "name,value\r\n\"quoted,field\",\"a\"\"b\"\r\n";
    auto rows = csv_cases(path, true);
    assert_equal(1u, rows.size());
    assert_equal("quoted,field", std::get<0>(rows[0].values)[0]);
    assert_equal("a\"b", std::get<0>(rows[0].values)[1]);
    std::ofstream(path) << "\"unterminated";
    assert_throws<std::runtime_error>([&] { csv_cases(path); });
    std::ofstream(path) << "one\ntwo\n";
    assert_equal(2u, line_cases(path).size());
  }
  [[= test{}]] void skip_in_exception_check() {
    assert_throws<std::runtime_error>([] { skip("still skipped"); });
  }
  [[= test{}]] void cleanup_failure_after_skip() { skip("skip before cleanup"); }
  [[= test{}]] void matcher_negative_paths() {
    assert_false(eq(1)(2).matched);
    assert_true(ne(1)(2).matched);
    assert_true(lt(2)(1).matched);
    assert_true(le(2)(2).matched);
    assert_true(ge(2)(2).matched);
    assert_true(anything()(0).matched);
    assert_true(is_null()(nullptr).matched);
    int value = 7;
    assert_true(not_null()(&value).matched);
    assert_true(is_true()(true).matched);
    assert_true(is_false()(false).matched);
    assert_true(is_empty()(std::vector<int>{}).matched);
    assert_true(negate(eq(1))(2).matched);
    assert_false(all_of(gt(0), lt(3))(4).matched);
    assert_false(all_of(eq(1), eq(2), eq(3))(0).matched);
    assert_true(any_of(eq(1), eq(2))(2).matched);
    assert_true(any_of(eq(1), eq(2), eq(3))(1).matched);
    assert_false(any_of(eq(1), eq(2))(3).matched);
    assert_true(contains_regex("[0-9]+")("abc123xyz").matched);
    assert_false(contains_regex("[0-9]+")("abc").matched);
    assert_throws<std::regex_error>([] { matches_regex("["); });
    assert_false(contains(eq(1))(std::vector{2, 3}).matched);
    assert_false(each(gt(0))(std::vector{1, -1}).matched);
    assert_true(each(gt(0))(std::vector<int>{}).matched);
    assert_false(elements_are(eq(1), eq(2))(std::vector{1}).matched);
    assert_false(elements_are(eq(1))(std::vector{1, 2}).matched);
    assert_false(elements_are(eq(1))(std::vector{2}).matched);
    assert_false(elements_are(eq(1), eq(2), eq(3))(std::vector{0, 2, 3}).matched);
    assert_true(elements_are()(std::vector<int>{}).matched);
    assert_false(unordered_elements_are(eq(1))(std::vector{1, 2}).matched);
    assert_false(unordered_elements_are(eq(1), eq(2))(std::vector{1, 3}).matched);
    assert_true(unordered_elements_are()(std::vector<int>{}).matched);
    assert_true(pointee(eq(7))(&value).matched);
    assert_false(pointee(eq(7))(static_cast<int*>(nullptr)).matched);
    assert_false(pointee(eq(8))(&value).matched);
    assert_false(optional_value(eq(1))(std::optional<int>{}).matched);
    assert_false(optional_value(eq(1))(std::optional{2}).matched);
    assert_false(variant_with<int>(eq(1))(std::variant<int, std::string>{"text"}).matched);
    assert_false(variant_with<int>(eq(1))(std::variant<int, std::string>{2}).matched);
    assert_true(near(1.0, 0.0)(1.0).matched);
    assert_true(near(100.0, 0.0, 0.1)(105.0).matched);
    assert_false(near(1.0, 0.1)(2.0).matched);
    assert_throws<std::invalid_argument>([] { near(1.0, -0.1); });
    assert_throws<std::invalid_argument>([] { near(1.0, 0.1, -0.1); });
    assert_true(ulp_near(1.0)(1.0).matched);
    assert_false(ulp_near(1.0)(std::numeric_limits<double>::infinity()).matched);
    assert_false(ulp_near(std::numeric_limits<double>::infinity())(1.0).matched);
    assert_false(ulp_near(1.0, 0)(std::nextafter(1.0, 2.0)).matched);
    assert_false(ulp_near(1.0, 1)(1.1).matched);
    assert_true(is_nan()(std::numeric_limits<double>::quiet_NaN()).matched);
    assert_true(throws<std::runtime_error>()([] { throw std::runtime_error("x"); }).matched);
    assert_false(throws<std::runtime_error>()([] {}).matched);
    assert_false(throws<std::runtime_error>()([] { throw 42; }).matched);
    assert_false(throws<std::runtime_error>(anything())([] {}).matched);
    assert_false(throws<std::runtime_error>(anything())([] { throw 42; }).matched);
    assert_true(does_not_throw()([] {}).matched);
    assert_false(does_not_throw()([] { throw 42; }).matched);
    assert_no_throw([] {});
    expect_no_throw([] {});
    assume(true);
    expect_that(1, eq(1));
    assert_true(difference(std::string("a"), std::string("ab")).find("Length differs") !=
                std::string::npos);
    assert_equal("", difference(std::string("same"), std::string("same")));
  }
  [[= test{}]] void matcher_exception_dispatch() {
    // Reuse the same callable type to check every exception outcome, including
    // skips that must propagate rather than becoming a matcher result.
    std::function<void()> callable;
    auto plain = throws<std::runtime_error>();
    auto detailed =
        throws<std::runtime_error>(property(&std::runtime_error::what, has_substr("expected")));
    auto no_throw = does_not_throw();
    callable = [] {};
    assert_false(plain(callable).matched);
    assert_false(detailed(callable).matched);
    assert_true(no_throw(callable).matched);
    callable = [] { throw std::runtime_error("expected error"); };
    assert_true(plain(callable).matched);
    assert_true(detailed(callable).matched);
    assert_false(no_throw(callable).matched);
    callable = [] { throw std::runtime_error("different error"); };
    assert_false(detailed(callable).matched);
    callable = [] { throw 42; };
    assert_false(plain(callable).matched);
    assert_false(detailed(callable).matched);
    assert_false(no_throw(callable).matched);
    callable = [] { skip("propagate matcher skip"); };
    if (const auto mode = std::getenv("ANNOTEST_MATCHER_SKIP")) {
      if (std::string_view(mode) == "throws") plain(callable);
      if (std::string_view(mode) == "throws-matcher") detailed(callable);
      if (std::string_view(mode) == "no-throw") no_throw(callable);
      fail("Matcher swallowed a skip");
    }
  }
  [[= test{}]] void ordered_matcher_boundaries() {
    auto ordered = elements_are(eq(1), eq(2), eq(3));
    assert_true(ordered(std::vector{1, 2, 3}).matched);
    auto missing = ordered(std::vector{1, 2});
    assert_false(missing.matched);
    assert_equal("Missing element at index 2", missing.explanation);
    auto extra = ordered(std::vector{1, 2, 3, 4});
    assert_false(extra.matched);
    assert_equal("Unexpected extra elements", extra.explanation);
    auto wrong = ordered(std::vector{1, 9, 3});
    assert_false(wrong.matched);
    assert_that(wrong.explanation, starts_with("Element 1:"));
    auto empty = ordered(std::vector<int>{});
    assert_false(empty.matched);
    assert_equal("Missing element at index 0", empty.explanation);
  }
  [[= test{}]] void source_empty_fields_and_headers() {
    auto path = current_test().temp_dir() / "boundaries.csv";
    std::ofstream(path) << "\"\"\n\"\",tail\n,\n";
    auto rows = csv_cases(path);
    assert_equal(3u, rows.size());
    assert_equal(std::vector<std::string>{""}, std::get<0>(rows[0].values));
    assert_equal((std::vector<std::string>{"", "tail"}), std::get<0>(rows[1].values));
    assert_equal((std::vector<std::string>{"", ""}), std::get<0>(rows[2].values));
    std::ofstream(path) << "header";
    assert_true(csv_cases(path, true).empty());
    std::ofstream(path) << "header\nvalue,";
    rows = csv_cases(path, true);
    assert_equal(1u, rows.size());
    assert_equal("1", rows[0].name);
    assert_equal((std::vector<std::string>{"value", ""}), std::get<0>(rows[0].values));
  }
  [[= test{}]] void range_difference_diagnostics() {
    using values = std::vector<std::string>;
    assert_equal("", difference(values{}, values{}));
    assert_equal("", difference(values{"same"}, values{"same"}));
    assert_equal("\nFirst difference at index 1: expected old, got new",
                 difference(values{"same", "old"}, values{"same", "new"}));
    assert_equal("\nLength differs after 1 elements",
                 difference(values{"same"}, values{"same", "extra"}));
    assert_equal("\nLength differs after 1 elements",
                 difference(values{"same", "extra"}, values{"same"}));
    assert_equal("\nLength differs after 0 elements", difference(values{}, values{"extra"}));
    assert_equal("\nLength differs after 0 elements", difference(values{"extra"}, values{}));
  }
  [[= test{}]] void matcher_failure() {
    expect_that(3, eq(2), "nonfatal matcher context");
    std::println("continued after expectation");
    assert_that(3, eq(2), "fatal matcher context");
    std::println("unreachable matcher body");
  }
  [[= test{}]] void matcher_failure_empty() {
    expect_that(3, eq(2));
    assert_that(3, eq(2));
  }
  [[= test{}]] void no_throw_failure() {
    expect_no_throw([] { throw 42; });
    assert_no_throw([] { throw 42; });
  }
  [[= test{}]] void value_printer_paths() {
    assert_equal("stream:7", print_value(StreamOnly{7}));
    auto text = print_value(std::array<Unprintable, 101>{});
    assert_that(text, has_substr("unprintable"));
    assert_that(text, has_substr("..."));
    assert_equal("[]", print_value(std::vector<Unprintable>{}));
    assert_true(difference(std::string("ab"), std::string("a")).find("Length differs") !=
                std::string::npos);
  }
  [[= test{}]] void source_edge_cases() {
    auto path = current_test().temp_dir() / "source.csv";
    assert_throws<std::runtime_error>([&] { csv_cases(path); });
    assert_throws<std::runtime_error>([&] { line_cases(path); });
    std::ofstream(path) << "";
    assert_true(csv_cases(path).empty());
    std::ofstream(path) << "a,b";
    assert_equal(1u, csv_cases(path).size());
    std::ofstream(path) << "\"multi\nline\",last\n";
    assert_equal("multi\nline", std::get<0>(csv_cases(path)[0].values)[0]);
    std::ofstream(path) << "\"closed\"x";
    assert_throws<std::runtime_error>([&] { csv_cases(path); });
    std::ofstream(path) << "bad\"quote";
    assert_throws<std::runtime_error>([&] { csv_cases(path); });
    std::ofstream(path) << "\"closed\"";
    assert_equal("closed", std::get<0>(csv_cases(path)[0].values)[0]);
    std::ofstream(path) << "a,\rnext\r";
    auto rows = csv_cases(path);
    assert_equal(2u, rows.size());
    assert_equal("", std::get<0>(rows[0].values)[1]);
    std::ofstream(path) << "one\r\ntwo";
    assert_equal(2u, line_cases(path).size());
  }
  [[= test{}]] void matcher_skip_paths() {
    auto mode = std::getenv("ANNOTEST_MATCHER_SKIP");
    if (mode && std::string_view(mode) == "throws")
      assert_that([] { skip("matcher skip"); }, throws<std::runtime_error>());
    if (mode && std::string_view(mode) == "throws-matcher")
      assert_that([] { skip("matcher skip"); }, throws<std::runtime_error>(anything()));
    if (mode && std::string_view(mode) == "no-throw") assert_no_throw([] { skip("matcher skip"); });
  }

  [[= test{}]] void contextual_failure() {
    scoped_trace outer("outer context");
    scoped_message inner("input row 7");
    expect_equal(std::string("abc"), std::string("axc"));
  }
  [[= test{}]] void crashes() {
    if (std::getenv("ANNOTEST_CRASH")) std::abort();
  }
  [[ = test{}, = timeout{0.2} ]] void deadline() {
    if (std::getenv("ANNOTEST_HANG")) std::this_thread::sleep_for(std::chrono::seconds(5));
  }
  [[= test{.disabled = true}]] void disabled() { record_property("ran", "yes"); }
  struct[[= nested{}]] Nested {
    [[= test{}]] void child() { assert_true(true); }
  };
};
struct[[= suite{.lifecycle = fixture_lifecycle::shared}]] SharedFixture {
  int value = 0;
  [[= test{}]] void first() { assert_equal(1, ++value); }
  [[= test{}]] void second() { assert_equal(2, ++value); }
};
struct InjectedFixture {
  std::string name;
  InjectedFixture(test_context& ctx) : name(ctx.name) {}
  [[= test{}]] void injected() { assert_equal("injected", name); }
};
struct Listener : test_listener {
  std::size_t starts = 0, ends = 0;
  void on_test_start(std::string_view, std::string_view) override { ++starts; }
  void on_test_end(const test_result&) override { ++ends; }
  void on_run_end(std::span<const test_result> results) override {
    if (starts != ends || ends != results.size())
      throw std::runtime_error("Unbalanced listener events");
  }
};
int main(int argc, char** argv) {
  // An independently failing child executable exercises the runner's protocol
  // validation. Parent invocations retain normal discovery and execution.
  if (const char* fault = std::getenv("ANNOTEST_WORKER_FAULT")) {
    for (int i = 1; i < argc; ++i) {
      std::string_view arg = argv[i];
      if (!arg.starts_with("--worker-result=")) continue;
      std::string mode = fault;
      if (mode == "missing") return 0;
      if (mode == "exit") return 2;
      std::ofstream output(std::string(arg.substr(16)), std::ios::binary);
      auto put = [&](std::string_view value) {
        std::uint64_t size = value.size();
        output.write(reinterpret_cast<const char*>(&size), sizeof(size));
        output.write(value.data(), value.size());
      };
      if (mode == "oversized") {
        std::uint64_t size = 65ULL * 1024 * 1024;
        output.write(reinterpret_cast<const char*>(&size), sizeof(size));
        return 0;
      }
      if (mode == "truncated") {
        put("AnnoTest worker 2");
        return 0;
      }
      if (mode == "protocol") {
        put("wrong protocol");
        return 0;
      }
      put("AnnoTest worker 2");
      put("0.001");
      put("worker output");
      put("worker error");
      put("0");
      put("");
      if (mode == "failure-count") {
        put("100001");
        return 0;
      }
      if (mode == "failure-type") {
        put("1");
        put("999");
        return 0;
      }
      if (mode == "properties") {
        put("0");
        put("100001");
        return 0;
      }
      if (mode == "exit-mismatch") {
        put("1");
        put("0");
        put("fixture failure");
        put("test");
        put("details");
        put("0");
        put("0");
        put("0");
        return 0;
      }
    }
  }

  add_listener(std::make_shared<Listener>());
  auto scenario = std::getenv("ANNOTEST_SCENARIO");
  if (scenario && std::string_view(scenario) == "shared")
    return run_tests<SharedFixture>(argc, argv);
  if (scenario && std::string_view(scenario) == "injected")
    return run_tests<InjectedFixture>(argc, argv);
  if (scenario && std::string_view(scenario) == "multiple")
    return run_tests<SharedFixture, InjectedFixture>(argc, argv);
  return run_tests<ParityTests>(argc, argv);
}
