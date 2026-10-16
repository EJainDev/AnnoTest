import annotest;
import std;

using namespace annotest;

struct ExpectationTests {
  int value = 1;
  [[= test{}]] void direct_fail() {
    add_failure_expect("explicit failure", std::source_location::current());
    std::println("continued");
  }
  [[= test{}]] void equal_pass() {
    expect_equal(7, 7);
    std::println("continued");
  }
  [[= test{}]] void equal_fail() {
    expect_equal(7, 8);
    std::println("continued");
  }
  [[= test{}]] void not_equal_pass() {
    expect_not_equal(7, 8);
    std::println("continued");
  }
  [[= test{}]] void not_equal_fail() {
    expect_not_equal(7, 7);
    std::println("continued");
  }
  [[= test{}]] void true_pass() {
    expect_true(42);
    std::println("continued");
  }
  [[= test{}]] void true_fail() {
    expect_true(0);
    std::println("continued");
  }
  [[= test{}]] void false_pass() {
    expect_false(0);
    std::println("continued");
  }
  [[= test{}]] void false_fail() {
    expect_false(42);
    std::println("continued");
  }
  [[= test{}]] void near_pass() {
    expect_near(1.0, 1.125, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_fail() {
    expect_near(1.0, 1.25, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_default_pass() {
    expect_near(1.0, 1.0005);
    std::println("continued");
  }
  [[= test{}]] void near_default_fail() {
    expect_near(1.0, 1.01);
    std::println("continued");
  }
  [[= test{}]] void near_nan_fail() {
    expect_near(0.0, std::numeric_limits<double>::quiet_NaN());
    std::println("continued");
  }
  [[= test{}]] void near_infinity_fail() {
    expect_near(0.0, std::numeric_limits<double>::infinity());
    std::println("continued");
  }
  [[= test{}]] void less_pass() {
    expect_less(1, 2);
    std::println("continued");
  }
  [[= test{}]] void less_fail() {
    expect_less(2, 2);
    std::println("continued");
  }
  [[= test{}]] void less_equal_pass() {
    expect_less_equal(2, 2);
    std::println("continued");
  }
  [[= test{}]] void less_equal_fail() {
    expect_less_equal(3, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_pass() {
    expect_greater(2, 1);
    std::println("continued");
  }
  [[= test{}]] void greater_fail() {
    expect_greater(2, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_pass() {
    expect_greater_equal(2, 2);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_fail() {
    expect_greater_equal(1, 2);
    std::println("continued");
  }
  [[= test{}]] void contains_pass() {
    expect_contains(std::vector{1, 2, 3}, 2);
    std::println("continued");
  }
  [[= test{}]] void contains_fail() {
    expect_contains(std::vector{1, 2, 3}, 4);
    std::println("continued");
  }
  [[= test{}]] void contains_empty_pass() {
    expect_contains(std::string{"abc"}, 'b');
    std::println("continued");
  }
  [[= test{}]] void contains_empty_fail() {
    expect_contains(std::vector<int>{}, 1);
    std::println("continued");
  }
  [[= test{}]] void null_pass() {
    expect_null(static_cast<int*>(nullptr));
    std::println("continued");
  }
  [[= test{}]] void null_fail() {
    expect_null(&value);
    std::println("continued");
  }
  [[= test{}]] void null_literal_pass() {
    expect_null(nullptr);
    std::println("continued");
  }
  [[= test{}]] void strings_pass() {
    expect_equal(std::string{"hello"}, std::string{"hello"});
    std::println("continued");
  }
  [[= test{}]] void strings_fail() {
    expect_equal(std::string{"< & \" ]] >"}, std::string{"different"});
    std::println("continued");
  }
  [[= test{}]] void mixed_types_pass() {
    expect_equal(3, 3.0);
    std::println("continued");
  }
  [[= test{}]] void mixed_types_fail() {
    expect_equal(3, 3.5);
    std::println("continued");
  }
  [[= test{}]] void near_mixed_pass() {
    expect_near(3, 3.125, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_mixed_fail() {
    expect_near(3, 3.25, 0.125);
    std::println("continued");
  }
  [[= test{}]] void near_negative_tolerance_fail() {
    expect_near(3.0, 3.0, -0.1);
    std::println("continued");
  }
  [[= test{}]] void less_nan_fail() {
    expect_less(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void less_equal_nan_fail() {
    expect_less_equal(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void greater_nan_fail() {
    expect_greater(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void greater_equal_nan_fail() {
    expect_greater_equal(std::numeric_limits<double>::quiet_NaN(), 1.0);
    std::println("continued");
  }
  [[= test{}]] void throws_pass() {
    expect_throws([] { throw std::runtime_error("expected"); });
    std::println("continued");
  }
  [[= test{}]] void throws_derived_pass() {
    expect_throws<std::exception>([] { throw std::runtime_error("derived"); });
    std::println("continued");
  }
  [[= test{}]] void throws_nonstd_pass() {
    expect_throws<int>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_missing_fail() {
    expect_throws([] {});
    std::println("continued");
  }
  [[= test{}]] void throws_wrong_fail() {
    expect_throws<std::runtime_error>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_pass() {
    expect_throws_exact<std::runtime_error>([] { throw std::runtime_error("exact"); });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_derived_fail() {
    expect_throws_exact<std::exception>([] { throw std::runtime_error("derived"); });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_missing_fail() {
    expect_throws_exact<std::exception>([] {});
    std::println("continued");
  }
  [[= test{}]] void throws_exact_wrong_fail() {
    expect_throws_exact<std::runtime_error>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void throws_exact_nonstd_pass() {
    expect_throws_exact<int>([] { throw 42; });
    std::println("continued");
  }
  [[= test{}]] void multiple_fail() {
    expect_equal(1, 2);
    expect_true(false);
    expect_throws([] {});
    std::println("continued");
  }
};

int main(int argc, char** argv) { return run_tests<ExpectationTests>(argc, argv); }
