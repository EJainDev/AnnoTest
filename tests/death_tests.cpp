#include <csignal>

import annotest;
import std;

using namespace annotest;

struct DeathTests {
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
    assertDeath([] { std::raise(SIGKILL); });
  }
  [[= test{}]] void expect_signal_pass() {
    expectDeath([] { std::raise(SIGKILL); });
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
