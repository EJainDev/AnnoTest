// Death helpers run a callable in a child process. Nonzero exit, a signal,
// or a thrown exception counts as death. Normal return / exit(0) fails.
import annotest;
import std;

using namespace annotest;

struct DeathTests {
  [[= test{}]] void nonzero_exit() {
    assert_death([] { std::_Exit(2); });
  }

  [[= test{}]] void throwing_child() {
    assert_death([] { throw std::runtime_error("child failure"); });
  }

  [[= test{}]] void expectation() {
    expect_death([] { std::_Exit(3); });
    // expect_death records failure without unwinding the body if the child survives.
    expect_true(true);
  }

  [[= test{}]] void child_state_is_isolated() {
    int value = 7;
    assert_death([&] {
      value = 99;
      std::_Exit(1);
    });
    assert_equal(7, value);
  }
};

int main(int argc, char** argv) { return run_tests<DeathTests>(argc, argv); }
