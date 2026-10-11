#include <csignal>
#include <cstdio>

import annotest;
import std;

using namespace annotest;

struct DeathTests {
  int fixture = 0;
  [[= before_each{}]] void setup() { fixture = 7; }
  [[= test<"quoted \"name\" café">{}]] void named_pass() {
    assertDeath([&] { std::_Exit(fixture == 7 ? 1 : 0); });
  }
  [[= test{}]] void multiple_pass() {
    assertDeath([] { std::_Exit(2); });
    expectDeath([] { std::_Exit(3); });
    assertDeath([] { throw 1; });
  }
  [[ = test{}, = parameterize{parameters(2), parameters(3)} ]] void parameter_pass(int value) {
    assertDeath([&] { std::_Exit(value); });
  }
  [[= test{}]] void assert_exit_pass() {
    assertDeath([] { std::_Exit(7); });
  }
  [[= test{}]] void expect_exit_pass() {
    expectDeath([] { std::_Exit(7); });
  }
  [[= test{}]] void assert_exception_pass() {
    assertDeath([] { throw 42; });
  }
  [[= test{}]] void expect_exception_pass() {
    expectDeath([] { throw 42; });
  }
  [[= test{}]] void assert_signal_pass() {
    assertDeath([] { std::raise(SIGTERM); });
  }
  [[= test{}]] void expect_signal_pass() {
    expectDeath([] { std::raise(SIGTERM); });
  }
  [[= test{}]] void assert_return_fail() {
    assertDeath([] {});
    std::println("continued");
  }
  [[= test{}]] void expect_return_fail() {
    expectDeath([] {});
    std::println("continued");
  }
  [[= test{}]] void assert_zero_exit_fail() {
    assertDeath([] { std::_Exit(0); });
  }
  [[= test{}]] void expect_zero_exit_fail() {
    expectDeath([] { std::_Exit(0); });
  }
  [[= test{}]] void nested_pass() {
    assertDeath([] {
      expectDeath([] {
        assertDeath([] {
          std::fputs("nested child\n", stdout);
          std::fflush(stdout);
          std::_Exit(7);
        });
        std::_Exit(8);
      });
      assertDeath([] {
        std::fputs("second nested child\n", stdout);
        std::fflush(stdout);
        std::_Exit(9);
      });
      std::fputs("outer survived\n", stdout);
      std::fflush(stdout);
      std::_Exit(10);
    });
  }
  [[= test{}]] void nested_return_fail() {
    assertDeath([] { assertDeath([] { std::_Exit(7); }); });
  }
  [[= test{}]] void isolation_pass() {
    int value = 1;
    assertDeath([&] {
      value = 99;
      std::_Exit(1);
    });
    assert_equal(1, value);
  }
};

int main(int argc, char** argv) { return run_tests<DeathTests>(argc, argv); }
