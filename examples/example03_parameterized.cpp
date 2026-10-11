/**
 * @example example03_parameterized.cpp
 *
 * This example demonstrates AnnoTest's parameterized testing:
 * - @cpp:struct[parameterize] — struct-based parameter injection
 * - @cpp:func[tuple] — compile-time tuple builder
 *
 * See :doc:`/api/testing/annotations` for full documentation.
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * Section: parameterize with single parameters.
 * Each tuple() call is one complete parameter set.
 */
struct ParameterizeBasic {
  [[ = Test{}, = parameterize{tuple(2, 3), tuple(5, 7), tuple(-1, -3)} ]] void addition(int a,
                                                                                        int b) {
    assert_equal(a + b, a + b);
    assert_true(a != 0 && b != 0);
  }

  [[ = Test{},
     = parameterize{tuple(1.0), tuple(2.5), tuple(0.001)} ]] void floating_point(double x) {
    assert_near(x, x);
    assert_true(x >= 0.0);
  }
};

/**
 * Section: Parameterized tests with lifecycle hooks.
 * before_each/after_each fire around each parameter set.
 */
struct ParameterizeWithHooks {
  int set_count = 0;

  [[= before_each{}]] void reset() { set_count = 0; }

  [[ = Test{}, = parameterize{tuple(100), tuple(200), tuple(300)} ]] void counting(int val) {
    ++set_count;
    assert_equal(val, val);
    assert_true(val % 100 == 0);
  }
};

int main(int argc, char** argv) { return test<ParameterizeBasic>(argc, argv); }
