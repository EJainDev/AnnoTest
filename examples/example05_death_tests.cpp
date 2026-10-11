// Death helpers run a callable in a child process. Nonzero exit, a signal,
// or a thrown exception counts as death. Normal return / exit(0) fails.
import annotest;
import std;

using namespace annotest;

struct DeathTests {
  [[= test{}]] void nonzero_exit() {
    assertDeath([] { std::_Exit(2); });
  }

  [[= test{}]] void throwing_child() {
    assertDeath([] { throw std::runtime_error("child failure"); });
  }

  [[= test{}]] void expectation() {
    expectDeath([] { std::_Exit(3); });
    // expectDeath records failure without unwinding the body if the child survives.
    expect_true(true);
  }

  [[= test{}]] void child_state_is_isolated() {
    int value = 7;
    assertDeath([&] {
      value = 99;
      std::_Exit(1);
    });
    assert_equal(7, value);
  }
};

int main(int argc, char** argv) { return run_tests<DeathTests>(argc, argv); }
