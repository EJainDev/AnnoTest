#include <annotest_contract.h>

import annotest;

using namespace annotest;

// Old broken:
unsigned int subtract(unsigned int a, unsigned int b) pre(a > b) { return a - b; }
// Fixed:
// unsigned int subtract(unsigned int a, unsigned int b) pre(a >= b) { return a - b; }

struct SubtractTests {
  [[
    = Test{}, = Parameterize{tuple(10u, 5u), tuple(10u, 0u), tuple(10u, 10u), tuple(1000005u, 5u)}
  ]] void subtraction_works(unsigned int a, unsigned int b) {
    const auto expected = a - b;
    assertEqual(expected, subtract(a, b));
  }
  [[= Test{}]] void subtraction_contract_works() {
    assertContractViolation([]() { subtract(5, 10); });
  }
};

int main(int argc, char** argv) { return test<SubtractTests>(argc, argv); }