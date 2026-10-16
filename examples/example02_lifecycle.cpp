// Each invocation gets a fresh fixture. Prepare per-test state in before_each.
// Hooks run around each test, including each parameterized instance.
import annotest;
import std;

using namespace annotest;

struct LifecycleTests {
  std::vector<int> values;
  int completed = 0;

  [[= before_each{}]] void setup() { values = {1, 2, 3}; }

  [[= after_each{}]] void cleanup() {
    values.clear();
    ++completed;
    // Captured output is available in the XML report's system-out element.
    std::println("Completed {} test instances", completed);
  }

  [[= test{}]] void append() {
    values.push_back(4);
    assert_equal(4u, values.size());
    assert_equal(4, values.back());
  }

  [[= test{}]] void fresh_fixture() {
    assert_equal(3u, values.size());
    assert_equal(1, values.front());
  }

  [[ = test{}, = parameterize{parameters(5), parameters(8)} ]] void append_value(int value) {
    values.push_back(value);
    assert_equal(4u, values.size());
    assert_equal(value, values.back());
  }
};

int main(int argc, char** argv) { return run_tests<LifecycleTests>(argc, argv); }
