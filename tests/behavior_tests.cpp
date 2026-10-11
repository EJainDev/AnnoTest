import annotest;
import std;

using namespace annotest;

namespace {
struct ScopeMarker {
  ~ScopeMarker() { std::println("destroyed"); }
};
struct NoncopyableContainer : std::vector<int> {
  NoncopyableContainer() : std::vector<int>{1, 2} {}
  NoncopyableContainer(const NoncopyableContainer&) = delete;
};
}

struct BehaviorTests {
  [[= test{}]] void assertion_unwinds_scope() {
    ScopeMarker marker;
    assert_equal(17, 29);
    std::println("unreachable");
  }
  [[= test{}]] void expectation_preserves_scope() {
    ScopeMarker marker;
    expect_equal(17, 29);
    std::println("continued");
  }
  [[= test{}]] void fatal_after_nonfatal() {
    expect_equal(17, 29);
    assert_equal(31, 43);
    expect_true(false);
    std::println("unreachable");
  }
  [[= test{}]] void passing_check_does_not_clear_failure() {
    expect_equal(17, 29);
    expect_equal(17, 17);
    std::println("continued");
  }
  [[= test{}]] void operands_evaluated_once() {
    int left = 0, right = 0;
    assert_equal(++left, ++right);
    assert_equal(1, left);
    assert_equal(1, right);
    expect_equal(++left, ++right);
    assert_equal(2, left);
    assert_equal(2, right);
  }
  [[= test{}]] void failure_operands_evaluated_once() {
    int left = 17, right = 29;
    expect_equal(left++, right++);
    std::println("counts:{}:{}", left, right);
  }
  [[= test{}]] void exception_callbacks_called_once() {
    int count = 0;
    assert_throws<std::runtime_error>([&] { ++count; throw std::runtime_error("once"); });
    assert_equal(1, count);
    expect_throws_exact<std::runtime_error>([&] { ++count; throw std::runtime_error("once"); });
    assert_equal(2, count);
  }
  [[= test{}]] void missing_exception_callback_called_once() {
    int count = 0;
    expect_throws([&] { ++count; });
    std::println("count:{}", count);
  }
  [[= test{}]] void explicit_source_location() {
#line 400 "explicit_location.cpp"
    const auto location = std::source_location::current();
    expect_equal(17, 29, location);
    assert_equal(31, 43, location);
#line 77 "behavior_tests.cpp"
  }
  [[= test{}]] void wrap_defers_invocation() {
    int count = 0;
    auto callback = wrap([&](int n) { ++count; return n; }, 7);
    assert_equal(0, count);
    assert_equal(7, callback());
    assert_equal(7, callback());
    assert_equal(2, count);
  }
  [[= test{}]] void wrap_copies_lvalue_arguments() {
    int original = 7;
    auto callback = wrap([](int& n) { return ++n; }, original);
    original = 99;
    assert_equal(8, callback());
    assert_equal(9, callback());
    assert_equal(99, original);
  }
  [[= test{}]] void wrap_preserves_explicit_reference() {
    int original = 7;
    auto callback = wrap([](int& n) -> int& { return n; }, std::ref(original));
    static_assert(std::is_same_v<decltype(callback()), int&>);
    callback() = 99;
    assert_equal(99, original);
  }
  [[= test{}]] void contains_noncopyable_container() {
    NoncopyableContainer values;
    assert_contains(values, 1);
    expect_contains(values, 2);
  }
  [[= test{}]] void string_embedded_null_is_compared() {
    assert_not_equal(std::string("a\0b", 3), std::string("a\0c", 3));
    expect_not_equal(std::string("a\0b", 3), std::string("a\0c", 3));
  }
};

int main(int argc, char** argv) { return run_tests<BehaviorTests>(argc, argv); }
