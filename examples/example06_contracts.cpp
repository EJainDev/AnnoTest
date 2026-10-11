/**
 * @example example06_contracts.cpp
 *
 * This example demonstrates AnnoTest's contract testing API:
 * - assert_contract_violation / expectContractViolation
 * - assert_no_contract_violation / expectNoContractViolation
 *
 * See :doc:`/api/testing/contracts` for C++26 contract documentation.
 */
#include <annotest_contract.h>

import annotest;

import std;

using namespace annotest;

int safe_divide(int a, int b) pre(a > 0) pre(b > 0) post(r : r > 0) {
  contract_assert(a != 0);
  contract_assert(b != 0);
  return a / b;
}

struct ContractViolationTests {
  /**
   * Section: assert_contract_violation — assert that a contract was triggered.
   * Wraps the violating code in a lambda; if no violation fires, the assertion fails.
   */
  [[= Test<"contract triggered on bad input">{}]] void division_by_zero() {
    assert_contract_violation([]() {
      safe_divide(10, 0);  // Triggers pre(b > 0)
    });
  }

  /**
   * Section: assert_contract_violation on a different contract.
   * The pre(a > 0) clause fires when a is negative.
   */
  [[= Test<"contract triggered on negative">{}]] void negative_input() {
    assert_contract_violation([]() {
      safe_divide(-5, 3);  // Triggers pre(a > 0)
    });
  }

  /**
   * Section: assert_no_contract_violation — assert that no contract fires.
   * Wraps valid code; if a contract does fire, the assertion fails.
   */
  [[= Test<"no contract on valid input">{}]] void valid_input() {
    assert_no_contract_violation([]() {
      safe_divide(10, 2);  // All contracts pass, returns 5
    });
  }
};

struct ExpectContractTests {
  /**
   * Section: expectContractViolation — same as assert but uses Abort.
   * If no violation fires, throws Abort (test aborts, suite continues).
   */
  [[= Test<"expect contract violation">{}]] void expect_violation() {
    expectContractViolation([]() {
      safe_divide(10, 0);  // Triggers pre(b > 0)
    });
    // If we reach here, the expectation was met
    assert_equal(1, 1);
  }

  /**
   * Section: expectNoContractViolation — same as assert but uses Abort.
   */
  [[= Test<"expect no contract violation">{}]] void expect_no_violation() {
    expectNoContractViolation([]() {
      safe_divide(6, 2);  // Returns 3 — all contracts pass
    });
  }
};

int main(int argc, char** argv) { return test<ContractViolationTests>(argc, argv); }
