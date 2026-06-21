import annotest;

import std;

using namespace annotest;

struct AssertionTestSuite {
  [[
    = Test{},
    = Parameterize{tuple(3, 3), tuple(0, 0), tuple(-1, -1), tuple(1, 1),
                   tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
                   tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())}
  ]] void AssertEqualInt(int a, int b) {
    assertEqual(a, b);
  }

  template <typename T = float>
  [[ = Test{}, = ParameterizeTemplate<int, double>{} ]] void AssertEqualTemplate() {
    assertEqual(42, 42);
  }
};

int main(int argc, char** argv) { return test<AssertionTestSuite>(argc, argv); }