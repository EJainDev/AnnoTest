// A realistic fixture combining checks, exceptions, names, skips and parameters.
import annotest;
import std;

using namespace annotest;

class Stack {
 public:
  void push(int value) { values.push_back(value); }
  int pop() {
    if (values.empty()) throw std::out_of_range("empty stack");
    int result = values.back();
    values.pop_back();
    return result;
  }
  bool empty() const { return values.empty(); }
  std::size_t size() const { return values.size(); }

 private:
  std::vector<int> values;
};

struct[[= suite<"Stack">{}]] StackTests {
  Stack fixture;

  [[= before_each{}]] void setup() { fixture = Stack{}; }
  [[= after_each{}]] void cleanup() { expect_true(fixture.size() <= 3); }

  [[= test<"last in first out">{}]] void lifo() {
    fixture.push(1);
    fixture.push(2);
    expect_equal(2u, fixture.size());
    assert_equal(2, fixture.pop());
    assert_equal(1, fixture.pop());
    expect_true(fixture.empty());
  }

  [[= test{}]] void empty_pop_throws() {
    assert_throws<std::out_of_range>([&] { fixture.pop(); });
    expect_true(fixture.empty());
  }

  [[ = test{},
     = parameterize{parameters(1), parameters(100), parameters(-50)} ]] void round_trip(int value) {
    fixture.push(value);
    assert_equal(value, fixture.pop());
    expect_true(fixture.empty());
  }

  [[= test{.disabled = true}]] void future_capacity_limit() {
    // Disabled tests are listed, but their bodies and hooks are not executed.
    assert_true(false);
  }
};

int main(int argc, char** argv) { return run_tests<StackTests>(argc, argv); }
