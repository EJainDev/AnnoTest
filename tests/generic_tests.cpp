import annotest;
import std;

using namespace annotest;

struct Calls {
  int comparisons = 0;
  int conversions = 0;
  int formats = 0;
  bool throw_comparison = false;
  bool throw_conversion = false;
  bool throw_format = false;
};

// Observe side effects even when a check unwinds the test body.
struct Trace {
  Calls& calls;
  ~Trace() { std::println("calls:{}:{}:{}", calls.comparisons, calls.conversions, calls.formats); }
};

struct BoolProxy {
  bool value;
  Calls* calls;
  explicit operator bool() const {
    ++calls->conversions;
    if (calls->throw_conversion) throw std::runtime_error("proxy conversion threw");
    return value;
  }
};

struct Value {
  int value;
  Calls* calls;
  BoolProxy compare(bool result) const {
    ++calls->comparisons;
    if (calls->throw_comparison) throw std::runtime_error("comparison threw");
    return {result, calls};
  }
  BoolProxy operator==(const Value& other) const { return compare(value == other.value); }
  BoolProxy operator!=(const Value& other) const { return compare(value != other.value); }
  BoolProxy operator<(const Value& other) const { return compare(value < other.value); }
  BoolProxy operator<=(const Value& other) const { return compare(value <= other.value); }
  BoolProxy operator>(const Value& other) const { return compare(value > other.value); }
  BoolProxy operator>=(const Value& other) const { return compare(value >= other.value); }
};

template <>
struct std::formatter<Value> : std::formatter<std::string> {
  auto format(const Value& value, std::format_context& context) const {
    ++value.calls->formats;
    if (value.calls->throw_format) throw std::runtime_error("formatter threw");
    return std::formatter<std::string>::format(std::format("Value({})", value.value), context);
  }
};

// std::find requires equality results implicitly convertible to bool. Keep the
// explicit proxy for the check helpers; use ordinary equality for containment.
struct SearchValue : Value {
  SearchValue(int value, Calls* calls) : Value{value, calls} {}
  bool operator==(const SearchValue& other) const {
    return static_cast<bool>(Value::operator==(other));
  }
};

template <>
struct std::formatter<SearchValue> : std::formatter<Value> {};

template <>
struct std::formatter<BoolProxy> : std::formatter<std::string> {
  auto format(const BoolProxy& value, std::format_context& context) const {
    ++value.calls->formats;
    return std::formatter<std::string>::format(value.value ? "proxy(true)" : "proxy(false)",
                                               context);
  }
};

// Exercise each public comparison with the same custom operand and result types.
void compare(int operation, bool nonfatal, Value left, Value right) {
  switch (operation) {
    case 0:
      if (nonfatal)
        expect_equal(left, right);
      else
        assert_equal(left, right);
      break;
    case 1:
      if (nonfatal)
        expect_not_equal(left, right);
      else
        assert_not_equal(left, right);
      break;
    case 2:
      if (nonfatal)
        expect_less(left, right);
      else
        assert_less(left, right);
      break;
    case 3:
      if (nonfatal)
        expect_less_equal(left, right);
      else
        assert_less_equal(left, right);
      break;
    case 4:
      if (nonfatal)
        expect_greater(left, right);
      else
        assert_greater(left, right);
      break;
    case 5:
      if (nonfatal)
        expect_greater_equal(left, right);
      else
        assert_greater_equal(left, right);
      break;
  }
}

void comparison_case(int operation, bool nonfatal, bool passing, Calls& calls) {
  // Passing pairs for ==, !=, <, <=, >, >=; failing pairs reverse the outcome.
  constexpr int left[] = {7, 7, 7, 7, 9, 7};
  constexpr int pass_right[] = {7, 9, 9, 7, 7, 7};
  constexpr int fail_right[] = {9, 7, 7, 6, 9, 9};
  compare(operation, nonfatal, {left[operation], &calls},
          {passing ? pass_right[operation] : fail_right[operation], &calls});
  std::println("continued");
}

struct GenericTests {
  [[
    = test{}, = parameterize{parameters(0, false), parameters(0, true), parameters(1, false),
                             parameters(1, true), parameters(2, false), parameters(2, true),
                             parameters(3, false), parameters(3, true), parameters(4, false),
                             parameters(4, true), parameters(5, false), parameters(5, true)}
  ]] void comparisons_pass(int operation, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    calls.throw_format = true;
    comparison_case(operation, nonfatal, true, calls);
  }
  [[
    = test{}, = parameterize{parameters(0, false), parameters(0, true), parameters(1, false),
                             parameters(1, true), parameters(2, false), parameters(2, true),
                             parameters(3, false), parameters(3, true), parameters(4, false),
                             parameters(4, true), parameters(5, false), parameters(5, true)}
  ]] void comparisons_fail(int operation, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    comparison_case(operation, nonfatal, false, calls);
  }
  [[
    = test{}, = parameterize{parameters(0, false), parameters(0, true), parameters(1, false),
                             parameters(1, true), parameters(2, false), parameters(2, true),
                             parameters(3, false), parameters(3, true), parameters(4, false),
                             parameters(4, true), parameters(5, false), parameters(5, true)}
  ]] void throwing_comparison(int operation, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    calls.throw_comparison = true;
    comparison_case(operation, nonfatal, true, calls);
  }
  [[
    = test{}, = parameterize{parameters(0, false), parameters(0, true), parameters(1, false),
                             parameters(1, true), parameters(2, false), parameters(2, true),
                             parameters(3, false), parameters(3, true), parameters(4, false),
                             parameters(4, true), parameters(5, false), parameters(5, true)}
  ]] void throwing_conversion(int operation, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    calls.throw_conversion = true;
    comparison_case(operation, nonfatal, true, calls);
  }
  [[
    = test{}, = parameterize{parameters(0, false), parameters(0, true), parameters(1, false),
                             parameters(1, true), parameters(2, false), parameters(2, true),
                             parameters(3, false), parameters(3, true), parameters(4, false),
                             parameters(4, true), parameters(5, false), parameters(5, true)}
  ]] void throwing_formatter(int operation, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    calls.throw_format = true;
    comparison_case(operation, nonfatal, false, calls);
  }
  [[
    = test{}, = parameterize{parameters(false, false), parameters(false, true),
                             parameters(true, false), parameters(true, true)}
  ]] void truth_pass(bool false_check, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    BoolProxy value{!false_check, &calls};
    if (false_check) {
      if (nonfatal)
        expect_false(value);
      else
        assert_false(value);
    } else {
      if (nonfatal)
        expect_true(value);
      else
        assert_true(value);
    }
    std::println("continued");
  }
  [[
    = test{}, = parameterize{parameters(false, false), parameters(false, true),
                             parameters(true, false), parameters(true, true)}
  ]] void truth_fail(bool false_check, bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    BoolProxy value{false_check, &calls};
    if (false_check) {
      if (nonfatal)
        expect_false(value);
      else
        assert_false(value);
    } else {
      if (nonfatal)
        expect_true(value);
      else
        assert_true(value);
    }
    std::println("continued");
  }
  [[ = test{}, = parameterize{parameters(false),
                              parameters(true)} ]] void contains_custom_values(bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    std::vector<SearchValue> values{{7, &calls}, {9, &calls}};
    if (nonfatal)
      expect_contains(values, SearchValue{9, &calls});
    else
      assert_contains(values, SearchValue{9, &calls});
    std::println("continued");
  }
  [[ = test{}, = parameterize{parameters(false),
                              parameters(true)} ]] void contains_custom_failure(bool nonfatal) {
    Calls calls;
    Trace trace{calls};
    std::vector<SearchValue> values{{7, &calls}, {9, &calls}};
    if (nonfatal)
      expect_contains(values, SearchValue{11, &calls});
    else
      assert_contains(values, SearchValue{11, &calls});
    std::println("continued");
  }
  [[= test{}]] void recovery_after_exception() {
    Calls calls;
    Trace trace{calls};
    comparison_case(0, false, true, calls);
  }
};

int main(int argc, char** argv) { return run_tests<GenericTests>(argc, argv); }
