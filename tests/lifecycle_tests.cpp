#include <cstdio>

import annotest;
import std;

using namespace annotest;

struct LifecycleTests {
  int state = 0;
  [[= before_each{}]] void setup() {
    std::println("setup {}", ++state);
    auto mode = std::getenv("ANNOTEST_SCENARIO");
    if (mode && std::string_view(mode) == "setup_exception")
      throw std::runtime_error("setup failed");
    if (mode && std::string_view(mode) == "setup_assertion") assert_true(false);
    if (mode && std::string_view(mode) == "setup_expectation") expect_true(false);
    if (mode && std::string_view(mode) == "setup_unknown") throw 42;
  }
  [[= after_each{}]] void cleanup() {
    std::println("cleanup {}", state);
    auto mode = std::getenv("ANNOTEST_SCENARIO");
    if (mode && std::string_view(mode) == "cleanup_exception")
      throw std::runtime_error("cleanup failed");
    if (mode && std::string_view(mode) == "cleanup_assertion") assert_true(false);
    if (mode && std::string_view(mode) == "cleanup_expectation") expect_true(false);
    if (mode && std::string_view(mode) == "cleanup_unknown") throw 42;
  }
  [[= test{}]] void passing() {
    std::println("body {}", state);
    assert_true(state > 0);
  }
  [[= test{}]] void second() {
    std::println("second {}", state);
    assert_true(state > 0);
  }
  [[= test{}]] void assertion_fail() {
    assert_equal(1, 2);
    std::println("unreachable");
  }
  [[= test{}]] void expectation_fail() {
    expect_equal(1, 2);
    std::println("continued");
  }
  [[= test{}]] void exception_fail() { throw std::runtime_error("exception < & \" ]]> message"); }
  [[= test{}]] void unknown_fail() { throw 42; }
  [[= test{.disabled = true}]] void disabled() { assert_true(false); }
  [[= test{}]] void output() {
    std::cout << "cout < & ]]>\n";
    std::cerr << "cerr < & ]]>\n";
    std::printf("printf\n");
    std::fprintf(stderr, "fprintf\n");
  }
  [[= test{}]] void large_output() {
    std::cout << std::string(200000, 'o');
    std::cerr << std::string(200000, 'e');
  }
  [[= test{}]] void unexpected_contract() { contract_violation_occurred = true; }
  [[= test{}]] void last() {
    std::println("last {}", state);
    assert_true(state > 0);
  }
};

int main(int argc, char** argv) {
  auto mode = std::getenv("ANNOTEST_SCENARIO");
  if (mode && std::string_view(mode) == "repeat") {
    char program[] = "LifecycleTests";
    char filter[] = "--test-name";
    char name[] = "LifecycleTests.assertion_fail";
    char* first[] = {program, filter, name};
    if (run_tests<LifecycleTests>(3, first) != 1) return 2;
  }
  return run_tests<LifecycleTests>(argc, argv);
}
