// A suite is a class with public annotated methods. main explicitly runs it.
import annotest;
import std;

using namespace annotest;

struct BasicTests {
  [[= test{}]] void addition() { assert_equal(4, 2 + 2); }

  [[= test{}]] void boolean_logic() {
    assert_true(4 > 2);
    assert_false(4 < 2);
  }

  [[= test<"subtraction works">{}]] void subtraction() { assert_equal(10, 15 - 5); }

  [[= test{}]] void exception_check() {
    const std::vector<int> values{1, 2, 3};
    assert_throws<std::out_of_range>([&] { static_cast<void>(values.at(10)); });
  }
};

int main(int argc, char** argv) { return run_tests<BasicTests>(argc, argv); }
