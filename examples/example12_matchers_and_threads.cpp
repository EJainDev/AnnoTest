import annotest;
import std;
using namespace annotest;

struct point {
  int x, y;
  friend void print_to(const point& value, std::ostream* out) {
    *out << "point(" << value.x << ", " << value.y << ')';
  }
};
struct matcher_tests {
  [[= test{}]] void values_and_ranges() {
    assert_all(
        "response",
        [] {
          assert_that(std::string{"ready-42"},
                      all_of(starts_with("ready"), contains_regex("[0-9]+")));
        },
        [] { assert_that(std::vector{2, 1}, unordered_elements_are(gt(0), eq(1))); },
        [] { assert_that(point{3, 4}, field(&point::x, eq(3))); },
        [] { assert_that(std::optional{7}, optional_value(near(7, 0))); });
  }
  [[= test{}]] void helper_threads() {
    scoped_trace trace("main thread");
    auto worker = test_thread([] {
      scoped_trace trace("background work");
      expect_equal(42, 6 * 7);
    });  // std::jthread joins before the test returns.
    assert_that(4, satisfies([](int value) { return value % 2 == 0; }, "an even number"));
  }
  [[= test{}]] void inspect_failures() {
    auto captured = capture_failures([] {
      expect_equal(1, 2, "an intentionally intercepted failure");
      assert_true(false);
    });
    assert_equal(2u, captured.size());
    assert_no_fatal_failure([] { assert_true(true); });
  }
};
int main(int argc, char** argv) { return run_tests<matcher_tests>(argc, argv); }
