#include <annotest_contract.h>

using namespace annotest;

int positive(int value) pre(value > 0) { return value; }

struct ConsumerTests {
  inline static bool ready = false;
  [[= before_all{}]] static void setup(test_context& context) {
    ready = true;
    assert_true(std::filesystem::exists(context.temp_dir()));
  }
  [[= after_all{}]] static void cleanup() { assert_true(std::exchange(ready, false)); }
  [[= test<"death check with spaces">{}]] void death() {
    assert_death([] { std::_Exit(7); });
  }
  [[= test{}]] void arithmetic() {
    assert_true(ready);
    assert_equal(4, 2 + 2);
  }

  [[= test<"contract handler">{}]] void contract_handler() {
    assert_equal(-1, assert_contract_violation([] { return positive(-1); }));
    expect_no_contract_violation([] { return positive(3); });
  }

  [[ = test{}, = parameterize{parameters(2, 4),
                              parameters(3, 6)} ]] void parameters_work(int input, int expected) {
    assert_equal(expected, input + input);
  }
};

int main(int argc, char** argv) { return run_tests<ConsumerTests>(argc, argv); }
