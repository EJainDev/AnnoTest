// --xml-output report.xml captures stdout/stderr and records check diagnostics.
// Set ANNOTEST_DEMONSTRATE_FAILURE=1 to see two nonfatal failures, one fatal
// failure, and continuation to the next test. That run intentionally returns 1.
import annotest;
import std;

using namespace annotest;

struct ReportingTests {
  [[= test{}]] void captured_output() {
    std::println("Output from the test body");
    std::println(std::cerr, "Diagnostic output from the test body");
    expect_equal(4, 2 + 2);
  }

  [[= test{}]] void failure_behavior() {
    if (std::getenv("ANNOTEST_DEMONSTRATE_FAILURE")) {
      expect_equal(5, 2 + 2);
      expect_true(false);
      std::println("Execution continues after failed expectations");
      assert_equal(6, 2 + 2);
      std::println("This statement is not reached");
    }
    assert_equal(4, 2 + 2);
  }

  [[= test{}]] void later_test() { std::println("Later tests still execute"); }
};

int main(int argc, char** argv) { return run_tests<ReportingTests>(argc, argv); }
