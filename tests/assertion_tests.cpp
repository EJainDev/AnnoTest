import annotest;

import std;

using namespace annotest;
template <typename T>
using lim = std::numeric_limits<T>;

struct AssertionTestSuite {
  [[= ParameterizeTemplate<int, double, int, int>]]

      template <typename A = int, typename B = int>
      [[
        = Test{},
        = dualParameterize(
            Pair<int, int>::InitM(tuple(3, 3), tuple(0, 0), tuple(-1, -1), tuple(1, 1),
                                  tuple(lim<int>::max(), lim<int>::max()),
                                  tuple(lim<int>::min(), lim<int>::min())),
            Pair<double, double>::InitM(tuple(3.14431764703641736497123649871264,
                                              3.14431764703641736497123649871264),
                                        tuple(0.0, 0.0), tuple(-1.0, -1.0), tuple(1.0, 1.0),
                                        tuple(lim<double>::max(), lim<double>::max()),
                                        tuple(lim<double>::min(), lim<double>::min())),
            Pair<int, unsigned int>::InitM(tuple(0, 0u), tuple(1, 1u), tuple(53, 53u),
                                           tuple(lim<int>::max(),
                                                 static_cast<unsigned int>(lim<int>::max()))))
      ]] void AssertEqual(A a, B b) {
    assertEqual(a, b);
  }

  template <typename A = int, typename B = int>
  [[
    = Test{}, = dualParameterize(
                  Pair<int, int>::InitM(tuple(3, 4), tuple(0, 1), tuple(-1, 0), tuple(1, 2),
                                        tuple(lim<int>::max(), lim<int>::min()),
                                        tuple(lim<int>::min(), lim<int>::max())),
                  Pair<double, double>::InitM(
                      tuple(3.14431764703641736497123649871264, 3.14431764703641736497123665),
                      tuple(0.0, 1.0), tuple(-1.0, -0.9999999999999999),
                      tuple(1.0, 1.0000000000000002), tuple(lim<double>::max(), lim<double>::min()),
                      tuple(lim<double>::min(), lim<double>::max())),
                  Pair<int, unsigned int>::InitM(tuple(0, 1u), tuple(1, 2u), tuple(53, 54u),
                                                 tuple(lim<int>::max(),
                                                       static_cast<unsigned int>(lim<int>::min()))))
  ]] void AssertNotEqual(A a, B b) {
    assertNotEqual(a, b);
  }

  template <typename A = int>
  [[
    = Test{},
    = dualParameterize(Pair<int>::InitM(tuple(3), tuple(lim<int>::max()), tuple(lim<int>::min())),
                       Pair<double>::InitM(tuple(3.14), tuple(lim<double>::max()),
                                           tuple(lim<double>::min())),
                       Pair<bool>::InitM(tuple(true)), Pair<char>::InitM(tuple('a'), tuple('z')))
  ]] void AssertTrue(A val) {
    assertTrue(val);
  }

  template <typename A = int>
  [[
    = Test{}, = dualParameterize(Pair<int>::InitM(tuple(0)), Pair<bool>::InitM(tuple(false)),
                                 Pair<void*>::InitM(tuple(nullptr)))
  ]] void AssertFalse(A val) {
    assertFalse(val);
  }

  template <typename A = int, typename B = int, typename C = A>
  [[
    = Test{}, = dualParameterize(Pair<int>::InitM(tuple(0, 0), tuple(1, 1, 0), tuple(-1, -1, 1),
                                                  tuple(lim<int>::max(), lim<int>::max()),
                                                  tuple(lim<int>::min(), lim<int>::min())))
  ]] void AssertNear(A a, B b, C tol = static_cast<C>(0.001)) {
    assertNear(a, b, tol);
  }
};

int main(int argc, char** argv) { return test<AssertionTestSuite>(argc, argv); }