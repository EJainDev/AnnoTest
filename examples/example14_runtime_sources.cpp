import annotest;
import std;
using namespace annotest;

std::vector<named_case<int, int>> sums() {
  return {case_of("small", 2, 4), case_of("large", 7, 14)};
}
struct source_tests {
  [[ = test{}, = parameter_source<&sums>{}, = tag<"data">{} ]] void twice(int input, int expected) {
    assert_equal(expected, input * 2);
  }

  [[= test{}]] void csv_and_lines(test_context& context) {
    auto file = context.temp_dir() / "cases.csv";
    std::ofstream(file) << "name,value\n\"row, one\",42\n";
    auto cases = csv_cases(file, true);
    assert_equal(1u, cases.size());
    assert_that(std::get<0>(cases.front().values), elements_are(eq("row, one"), eq("42")));
    assert_equal(2u, line_cases(file).size());
  }
  [[ = test{}, = test_factory{} ]] auto dynamic_children() {
    return std::vector<dynamic_case>{{"positive", [] { assert_greater(7, 0); }},
                                     {"negative", [] { assert_less(-7, 0); }}};
  }
};
struct summary_listener : test_listener {
  void on_run_end(std::span<const test_result> results) override {
    std::println("Custom reporter received {} results", results.size());
  }
};
int main(int argc, char** argv) {
  add_listener(std::make_shared<summary_listener>());
  register_test("runtime", "registered", [] { assert_that(42, gt(0)); }, {"data"});
  return run_tests<source_tests>(argc, argv);
}
