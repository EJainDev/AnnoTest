/**
 * @example example02_lifecycle.cpp
 *
 * This example demonstrates the test lifecycle annotations:
 * - @cpp:struct[before_all] — runs once before all tests
 * - @cpp:struct[before_each] — runs before each individual test
 * - @cpp:struct[after_each] — runs after each individual test
 * - @cpp:struct[after_all] — runs once after all tests complete
 *
 * These are useful for:
 * - Setting up shared state (before_all / after_all)
 * - Resetting per-test fixtures (before_each / after_each)
 *
 * The execution order is:
 * 1. before_all()
 * 2. (before_each(), test(), after_each()) × N tests
 * 3. after_all()
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

/**
 * A counter shared across the entire test suite.
 * Demonstrates that before_all/after_all run only once.
 */
int call_count = 0;

struct LifecycleTests {
  /**
   * before_all: called once, before the first test.
   * If this throws, the entire suite is aborted.
   */
  [[= before_all{}]] void setUp() {
    call_count = 0;
    std::cout << "[before_all] Setup shared resources\n";
  }

  /**
   * before_each: called before every test.
   * Resets the counter for each test.
   */
  [[= before_each{}]] void resetCounters() { call_count = 0; }

  /**
   * after_each: called after every test.
   * If this throws, only a warning is printed (suite continues).
   */
  [[= after_each{}]] void reportCounts() {
    std::cout << "[after_each] counter = " << call_count << "\n";
  }

  /**
   * after_all: called once, after the last test.
   * If this throws, only a warning is printed.
   */
  [[= after_all{}]] void tearDown() { std::cout << "[after_all] Cleaning up shared resources\n"; }

  /**
   * Demonstrates before_each runs before this test.
   */
  [[= Test{}]] void test_one() {
    ++call_count;
    assert_equal(1, call_count);
  }

  /**
   * Demonstrates before_each runs before every test, including this one.
   */
  [[= Test{}]] void test_two() {
    ++call_count;
    ++call_count;
    assert_equal(2, call_count);
  }

  /**
   * Demonstrates after_each runs after this test.
   */
  [[= Test{}]] void test_three() {
    ++call_count;
    ++call_count;
    ++call_count;
    assert_equal(3, call_count);
  }
};

int main(int argc, char** argv) { return test<LifecycleTests>(argc, argv); }
