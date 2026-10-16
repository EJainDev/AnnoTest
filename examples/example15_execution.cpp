import annotest;
import std;
using namespace annotest;

bool even_invocation(const test_context& context) { return context.repetition % 2 == 0; }
struct execution_tests {
  [[ = test{.repetitions = 2}, = tag<"fast">{} ]] void passing(test_context& context) {
    record_property("invocation", std::to_string(context.repetition + 1));
    assert_equal(4, 2 + 2);
  }
  [[ = test{.repetitions = 2},
     = enabled_if<&even_invocation>{"odd invocation"} ]] void conditional() {
    assert_true(true);
  }
  [[ = test{}, = timeout{5}, = tag<"isolated">{} ]] void deadline() {
    assert_that(current_test().name, eq("deadline"));
    std::println("Captured from an isolated worker");
  }
};
int main(int argc, char** argv) { return run_tests<execution_tests>(argc, argv); }
