// Include the handler in exactly one translation unit of this executable.
// CMake enables -fcontracts -fcontract-evaluation-semantic=observe for this target.
#include <annotest_contract.h>

using namespace annotest;

// Observe semantics continue after a violation. The body must remain safe
// for invalid inputs: avoid division by zero or unchecked container access.
int positive(int value) pre(value > 0) { return value; }
int positive_result(int value) post(result : result > 0) { return value; }

struct ContractTests {
  [[= test{}]] void precondition_violation() {
    const int result = assert_contract_violation([] { return positive(-1); });
    assert_equal(-1, result);
  }

  [[= test{}]] void postcondition_violation() {
    expect_contract_violation([] { return positive_result(-1); });
  }

  [[= test{}]] void inline_contract_violation() {
    assert_contract_violation([] { contract_assert(false); });
  }

  [[= test{}]] void valid_input() {
    assert_equal(3, assert_no_contract_violation([] { return positive(3); }));
    expect_no_contract_violation([] { contract_assert(true); });
  }
};

int main(int argc, char** argv) { return run_tests<ContractTests>(argc, argv); }
