import annotest;
import std;

using namespace annotest;

struct AssertionTests {
  int value = 1;
  [[= test{}]] void direct_fail() {
    add_failure("explicit failure", std::source_location::current());
    std::println("continued");
  }
  [[= test{}]] void equal_pass() {
    assert_equal(7, 7);
    std::println("continued");
  }
  [[= test{}]] void equal_fail() {
    assert_equal(7, 8);
    std::println("continued");
  }
  [[= test{}]] void not_equal_pass() {
    assert_not_equal(7, 8);
    std::println("continued");
  }
  [[= test{}]] void not_equal_fail() {
    assert_not_equal(7, 7);
    std::println("continued");
  }
  [[= test{}]] void true_pass() {
    assert_true(42);
    std::println("continued");
  }
  [[= test{}]] void true_fail() {
    assert_true(0);
    std::println("continued");
  }
  [[= test{}]] void false_pass() {
    assert_false(0);
    std::println("continued");
  }
  [[= test{}]] void false_fail() {
    assert_false(42);
    std::println("continued");
  }
  [[= test{}]] void near_pass() {
    assert_near(1.0, 1.125, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_fail() {
    assert_near(1.0, 1.25, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_default_pass() {
    assert_near(1.0, 1.0005);
    std::println("continued");
  }
  [[= test{}]] void near_default_fail() {
    assert_near(1.0, 1.01);
    std::println("continued");
  }
  [[= test{}]] void near_nan_fail() {
    assert_near(0.0, std::numeric_limits<double>::quiet_NaN());
    std::println("continued");
  }
  [[= test{}]] void near_infinity_fail() {
    assert_near(0.0, std::numeric_limits<double>::infinity());
    std::println("continued");
  }
  [[= test{}]] void less_pass() {
    assert_less(1, 2);
    std::println("continued");
  }
  [[= test{}]] void less_fail() {
    assert_less(2, 2);
    std::println("continued");
  }
  [[= test{}]] void less_equal_pass() {
    assert_less_equal(2, 2);
    std::println("continued");
  }
  [[= test{}]] void less_equal_fail() {
    assert_less_equal(3, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_pass() {
    assert_greater(2, 1);
    std::println("continued");
  }
  [[= test{}]] void greater_fail() {
    assert_greater(2, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_pass() {
    assert_greater_equal(2, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_fail() {
    assert_greater_equal(1, 2);
    std::println("continued");
  }
  [[= test{}]] void contains_pass() {
    assert_contains(std::vector{1, 2, 3}, 2);
    std::println("continued");
  }
  [[= test{}]] void contains_fail() {
    assert_contains(std::vector{1, 2, 3}, 4);
    std::println("continued");
  }
  [[= test{}]] void contains_empty_pass() {
    assert_contains(std::string{"abc"}, 'b');
    std::println("continued");
  }
  [[= test{}]] void contains_empty_fail() {
    assert_contains(std::vector<int>{}, 1);
    std::println("continued");
  }
  [[= test{}]] void null_pass() {
    assert_null(static_cast<int*>(nullptr));
    std::println("continued");
  }
  [[= test{}]] void null_fail() {
    assert_null(&value);
    std::println("continued");
  }
  [[= test{}]] void null_literal_pass() {
    assert_null(nullptr);
    std::println("continued");
  }
  [[= test{}]] void strings_pass() {
    assert_equal(std::string{"hello"}, std::string{"hello"});
    std::println("continued");
  }
  [[= test{}]] void strings_fail() {
    assert_equal(std::string{"< & \" ]] >"}, std::string{"different"});
    std::println("continued");
  }
  [[= test{}]] void mixed_types_pass() {
    assert_equal(3, 3.0);
    std::println("continued");
  }
  [[= test{}]] void mixed_types_fail() {
    assert_equal(3, 3.5);
    std::println("continued");
  }
  [[= test{}]] void near_mixed_pass() {
    assert_near(3, 3.125, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_mixed_fail() {
    assert_near(3, 3.25, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_negative_tolerance_fail() {
    assert_near(3.0, 3.0, -0.1);
    std::println("continued");
  }
  [[= test{}]] void less_nan_fail() {
    assert_less(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void less_equal_nan_fail() {
    assert_less_equal(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void greater_nan_fail() {
    assert_greater(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_nan_fail() {
    assert_greater_equal(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void throws_pass() {
    assert_throws([] { throw std::runtime_error("expected"); });
    std::println("continued");
  }
  [[= test{}]] void throws_derived_pass() {
    assert_throws<std::exception>([] { throw std::runtime_error("derived"); });
    std::println("continued");
  }
  [[= test{}]] void throws_nonstd_pass() {
    assert_throws<int>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_missing_fail() {
    assert_throws([] {});
    std::println("continued");
  }
  [[= test{}]] void throws_wrong_fail() {
    assert_throws<std::runtime_error>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_pass() {
    assert_throws_exact<std::runtime_error>([] { throw std::runtime_error("exact"); });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_derived_fail() {
    assert_throws_exact<std::exception>([] { throw std::runtime_error("derived"); });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_missing_fail() {
    assert_throws_exact<std::exception>([] {});
    std::println("continued");
  }
  [[= test{}]] void throws_exact_wrong_fail() {
    assert_throws_exact<std::runtime_error>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_nonstd_pass() {
    assert_throws_exact<int>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void wrap_pass() {
    auto throwing = wrap(
        [](int a, std::string b) {
          if (a == 7 && b == "argument") throw std::runtime_error("wrapped");
        },
        7, std::string{"argument"});
    assert_throws<std::runtime_error>(throwing);
    auto moved = wrap([](std::unique_ptr<int>& p) { return *p; }, std::make_unique<int>(9));
    assert_equal(9, moved());
    assert_equal(9, moved());
  }
};

int main(int argc, char** argv) { return run_tests<AssertionTests>(argc, argv); }
