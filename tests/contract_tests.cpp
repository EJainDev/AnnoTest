#include <annotest_contract.h>

using namespace annotest;

int positive(int n) pre(n > 0) { return n; }
int positive_result(int n) post(r : r > 0) { return n; }
int handler_calls = 0;
void custom(const std::contracts::contract_violation&) { ++handler_calls; }

struct ContractTests {
  [[= after_each{}]] void cleanup() { custom_handler.store(nullptr); }
  [[= test{}]] void assert_pre_pass() {
    assert_equal(-1, assert_contract_violation([] { return positive(-1); }));
  }
  [[= test{}]] void expect_pre_pass() {
    assert_equal(-1, expect_contract_violation([] { return positive(-1); }));
  }
  [[= test{}]] void assert_post_pass() {
    assert_equal(-1, assert_contract_violation([] { return positive_result(-1); }));
  }
  [[= test{}]] void expect_post_pass() {
    assert_equal(-1, expect_contract_violation([] { return positive_result(-1); }));
  }
  [[= test{}]] void assert_inline_pass() {
    assert_contract_violation([] { contract_assert(false); });
  }
  [[= test{}]] void expect_inline_pass() {
    expect_contract_violation([] { contract_assert(false); });
  }
  [[= test{}]] void assert_no_violation_pass() {
    assert_equal(3, assert_no_contract_violation([] { return positive(3); }));
  }
  [[= test{}]] void expect_no_violation_pass() {
    assert_equal(3, expect_no_contract_violation([] { return positive(3); }));
  }
  [[= test{}]] void assert_no_violation_void_pass() {
    assert_no_contract_violation([] { contract_assert(true); });
  }
  [[= test{}]] void expect_no_violation_void_pass() {
    expect_no_contract_violation([] { contract_assert(true); });
  }
  [[= test{}]] void assert_missing_fail() {
    assert_contract_violation([] {});
    std::println("continued");
  }
  [[= test{}]] void expect_missing_fail() {
    expect_contract_violation([] {});
    std::println("continued");
  }
  [[= test{}]] void assert_unwanted_fail() {
    assert_no_contract_violation([] { contract_assert(false); });
    std::println("continued");
  }
  [[= test{}]] void expect_unwanted_fail() {
    expect_no_contract_violation([] { contract_assert(false); });
    std::println("continued");
  }
  [[= test{}]] void assert_missing_value_fail() {
    (void)assert_contract_violation([] { return 7; });
  }
  [[= test{}]] void expect_missing_value_fail() {
    assert_equal(7, expect_contract_violation([] { return 7; }));
    std::println("continued");
  }
  [[= test{}]] void assert_unwanted_value_fail() {
    (void)assert_no_contract_violation([] { return positive(-1); });
  }
  [[= test{}]] void expect_unwanted_value_fail() {
    assert_equal(-1, expect_no_contract_violation([] { return positive(-1); }));
    std::println("continued");
  }
  [[= test{}]] void reset_pass() {
    contract_violation_occurred = true;
    assert_no_contract_violation([] {});
    assert_false(contract_violation_occurred);
    int calls = 0;
    assert_equal(9, assert_no_contract_violation([&] {
                   ++calls;
                   return 9;
                 }));
    expect_equal(9, expect_no_contract_violation([&] {
                   ++calls;
                   return 9;
                 }));
    assert_equal(2, calls);
    assert_contract_violation([] { contract_assert(false); });
    assert_false(contract_violation_occurred);
    expect_contract_violation([] { contract_assert(false); });
    assert_false(contract_violation_occurred);
  }
  [[= test{}]] void custom_handler_pass() {
    handler_calls = 0;
    custom_handler.store(custom);
    assert_contract_violation([] {
      contract_assert(false);
      contract_assert(false);
    });
    assert_equal(2, handler_calls);
    custom_handler.store(nullptr);
    expect_contract_violation([] { contract_assert(false); });
    assert_equal(2, handler_calls);
  }
  [[= test{}]] void callable_exception_fail() {
    assert_no_contract_violation([] { throw std::runtime_error("callable exception"); });
  }
  [[= test{}]] void unexpected_fail() {
    contract_assert(false);
    std::println("continued");
  }
};

int main(int argc, char** argv) { return run_tests<ContractTests>(argc, argv); }
