import annotest;

import std;

using namespace annotest;

template <typename T>
using lim = std::numeric_limits<T>;

static constexpr auto params = parameters(3, 3);
using parameterize_type = template_params<int, int>;
static constexpr auto parameterization_pair =
    parameterize_pair<parameterize_type, decltype(params)>(params);

struct AssertionTestSuite {
  template <typename A = int, typename B = int>
  [[ = test{}, = parameterize_dual(parameterization_pair) ]] void AssertEqual(A a, B b) {
    assertEqual(a, b);
  }
};

int main(int argc, char** argv) { return run_tests<AssertionTestSuite>(argc, argv); }