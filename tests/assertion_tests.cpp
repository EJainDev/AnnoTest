import annotest;

import std;

using namespace annotest;

struct AssertionTestSuite {
  template <typename A = int, typename B = int>
  [[
    = Test{},
    = makeTParamMatrix(
        Pair<int, int>::InitM(
            tuple(3, 3), tuple(0, 0), tuple(-1, -1), tuple(1, 1),
            tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
            tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())),
        Pair<double, double>::InitM(
            tuple(3.14, 3.14), tuple(0.0, 0.0), tuple(-1.0, -1.0), tuple(1.0, 1.0),
            tuple(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()),
            tuple(std::numeric_limits<double>::min(), std::numeric_limits<double>::min())))
  ]] void AssertEqual(A a, B b) {
    assertEqual(a, b);
  }
};

int main(int argc, char** argv) { return test<AssertionTestSuite>(argc, argv); }