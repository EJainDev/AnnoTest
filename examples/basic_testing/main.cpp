#include <annotest_contract.h>

import annotest;

using namespace annotest;

unsigned int subtract(unsigned int a, unsigned int b) pre(a > b) { return a - b; }

struct SubtractTests {
  [[= Test{}]] void subtraction_works() { assertEqual(5, subtract(10, 5)); }
  [[= Test{}]] void subtraction_contract_works() {
    assertContractViolation([]() { subtract(5, 10); });
  }
};

int main(int argc, char** argv) { return test<SubtractTests>(argc, argv); }