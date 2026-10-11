// parameters(...) describes one argument set, including the expected result.
import annotest;
import std;

using namespace annotest;

constexpr int add(int a, int b) { return a + b; }

// Append each value in the second set to the argument tuple in the first set.
constexpr auto products = parameters(2).cross(parameters(3, 5));

struct ParameterizedTests {
  [[
    = test{}, = parameterize{parameters(2, 3, 5), parameters(5, 7, 12), parameters(-1, -3, -4)}
  ]] void addition(int a, int b, int expected) {
    assert_equal(expected, add(a, b));
  }

  [[
    = test{}, = parameterize{parameters(1.0, 1.0), parameters(2.5, 6.25), parameters(0.5, 0.25)}
  ]] void square(double input, double expected) {
    assert_near(expected, input * input, 0.0001);
  }

  [[ = test{}, = parameterize(products) ]] void multiplication(int a, int b) {
    int repeated_sum = 0;
    for (int i = 0; i < b; ++i) repeated_sum += a;
    assert_equal(repeated_sum, a * b);
  }
};

// --test-name ParameterizedTests.addition.2 selects only the second argument set.
int main(int argc, char** argv) { return run_tests<ParameterizedTests>(argc, argv); }
