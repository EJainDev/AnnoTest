import annotest;
import std;
using namespace annotest;

namespace custom {
struct Printed {
  int value;
  bool operator==(const Printed&) const = default;
  friend void PrintTo(const Printed& item, std::ostream* out) {
    *out << "Printed(" << item.value << ")";
  }
  friend std::ostream& operator<<(std::ostream& out, const Printed&) {
    return out << "stream fallback";
  }
};
struct Counted {
  int value;
  static inline int prints = 0;
  bool operator==(const Counted&) const = default;
  friend void PrintTo(const Counted& item, std::ostream* out) {
    ++prints;
    *out << "Counted(" << item.value << ")";
  }
};
}  // namespace custom

struct DiagnosticTests {
  [[= test{}]] void string_success() {
    char first[] = "same", second[] = "same";
    assert_not_equal(static_cast<char*>(first), static_cast<char*>(second));
    assert_streq(first, second);
    expect_streq(first, second);
    assert_strne("a", "b");
    expect_strne("a", "b");
    assert_strcaseeq("Hello", "hELLo");
    expect_strcaseeq("Hello", "hELLo");
    assert_strcasene("Hello", "world");
    expect_strcasene("Hello", "world");
    assert_streq(nullptr, nullptr);
    expect_streq(nullptr, nullptr);
    assert_strcaseeq(nullptr, nullptr);
    expect_strcaseeq(nullptr, nullptr);
    assert_strne(nullptr, "");
    expect_strne("", nullptr);
    assert_strcasene(nullptr, "");
    expect_strcasene("", nullptr);
    assert_streq("", "");
    assert_strcaseeq("", "");
    assert_strcaseeq("AbC123!", "aBc123!");
    assert_strcasene("ab", "abc");
    assert_strcasene("abc", "ab");
    const char high[] = {static_cast<char>(0xff), 0};
    assert_strcaseeq(high, high);
    assert_strcasene(high, "A");
    assert_streq("abc\0different", "abc\0tail");
  }
  [[= test{}]] void wide_string_success() {
    wchar_t first[] = L"same", second[] = L"same";
    assert_streq(first, second);
    expect_streq(first, second);
    assert_strne(L"a", L"b");
    expect_strne(L"a", L"b");
    assert_strcaseeq(L"Hello", L"hELLo");
    expect_strcaseeq(L"Hello", L"hELLo");
    assert_strcasene(L"Hello", L"world");
    expect_strcasene(L"Hello", L"world");
    const wchar_t* empty = nullptr;
    assert_streq(empty, nullptr);
    expect_streq(nullptr, empty);
    assert_strcaseeq(empty, nullptr);
    expect_strcaseeq(nullptr, empty);
    assert_strne(nullptr, L"");
    expect_strne(L"", nullptr);
    assert_strcasene(nullptr, L"");
    expect_strcasene(L"", nullptr);
    assert_streq(L"caféΩ😀", L"caféΩ😀");
    assert_strcasene(L"É", L"é");  // Case folding is intentionally ASCII only.
  }
  [[= test{}]] void string_diagnostic_boundaries() {
    auto check = [](auto expected, auto actual) {
      auto failures = capture_failures([&] { assert_streq(expected, actual); });
      assert_equal(1u, failures.size());
      return failures[0].message;
    };
    assert_that(check("abc", "ab"), has_substr("First difference at index 2"));
    assert_that(check("ab", "abc"), has_substr("First difference at index 2"));
    assert_that(check("", "a"), has_substr("First difference at index 0"));
    assert_that(check(nullptr, ""), has_substr("expected nullptr, got \"\""));
    assert_that(check("", nullptr), has_substr("expected \"\", got nullptr"));
    auto failures = capture_failures([] { expect_strcaseeq("Ab", "aC"); });
    assert_equal(1u, failures.size());
    assert_that(failures[0].message, has_substr("First difference at index 1"));
    failures = capture_failures([] { expect_strcaseeq("ABC", "ab"); });
    assert_that(failures[0].message, has_substr("First difference at index 2"));
    char first[] = "same", second[] = "same";
    failures = capture_failures([&] { expect_equal(first, second); });
    assert_equal(1u, failures.size());
    assert_that(failures[0].message, has_substr("Pointer comparison"));
    assert_that(failures[0].message, has_substr(std::format("{}", static_cast<void*>(first))));
    assert_that(failures[0].message, has_substr(std::format("{}", static_cast<void*>(second))));
  }
  [[= test{}]] void string_failures_without_messages() {
    auto failures = capture_failures([] {
      expect_streq("abc", "xyz");
      expect_strne("abc", "abc");
      expect_strcaseeq("abc", "XYZ");
      expect_strcasene("abc", "ABC");
    });
    assert_equal(4u, failures.size());
    auto fatal = [](auto callable) {
      auto failures = capture_failures(callable);
      assert_equal(1u, failures.size());
      assert_equal(failure_types::assertion, failures[0].type);
    };
    fatal([] { assert_streq("abc", "xyz"); });
    fatal([] { assert_strne("abc", "abc"); });
    fatal([] { assert_strcaseeq("abc", "XYZ"); });
    fatal([] { assert_strcasene("abc", "ABC"); });
  }
  [[= test{}]] void safe_value_printing() {
    const char* empty = nullptr;
    const wchar_t* wide_empty = nullptr;
    assert_equal("nullptr", print_value(empty));
    assert_equal("nullptr", print_value(wide_empty));
    assert_equal("nullptr", print_value(nullptr));
    assert_equal("\"line\\n\\r\\t\\\\\\\"\\x01\\x7f\"", print_value("line\n\r\t\\\"\x01\x7f"));
    char bounded[] = {'a', 'b'};
    assert_equal("\"ab\"", print_value(bounded));
    assert_equal("\"caféΩ😀\"", print_value(L"caféΩ😀"));
    assert_equal("\"caféΩ😀\"", print_value(std::wstring(L"caféΩ😀")));
    assert_equal("\"wide\"", print_value(std::wstring_view(L"wide")));
    assert_equal("\"a\\0b\"", print_value(std::wstring({L'a', L'\0', L'b'})));
    std::wstring invalid(1, static_cast<wchar_t>(0xd800));
    assert_that(print_value(invalid), has_substr("d800"));
    if constexpr (sizeof(wchar_t) == 4) {
      invalid[0] = static_cast<wchar_t>(0x110000);
      assert_that(print_value(invalid), has_substr("110000"));
    }
  }
  [[= test{}]] void custom_printers() {
    assert_equal("Printed(7)", print_value(custom::Printed{7}));
    assert_equal("[Printed(1), Printed(2)]",
                 print_value(std::vector{custom::Printed{1}, custom::Printed{2}}));
    assert_that(difference(std::vector{custom::Printed{1}}, std::vector{custom::Printed{2}}),
                has_substr("expected Printed(1), got Printed(2)"));
    custom::Counted::prints = 0;
    assert_equal(custom::Counted{1}, custom::Counted{1});
    assert_equal(0, custom::Counted::prints);
    auto failures = capture_failures([] { expect_equal(custom::Counted{1}, custom::Counted{2}); });
    assert_equal(1u, failures.size());
    assert_equal(2, custom::Counted::prints);
    assert_that(failures[0].message, has_substr("expected Counted(1), got Counted(2)"));
  }
  [[
    = test{}, = parameterize{parameters(0), parameters(1), parameters(2), parameters(3),
                             parameters(4), parameters(5), parameters(6), parameters(7)}
  ]] void string_failure(int operation) {
    switch (operation) {
      case 0:
        expect_streq("abc", "axc", "nonfatal context");
        break;
      case 1:
        expect_strne("same", "same", "nonfatal context");
        break;
      case 2:
        expect_strcaseeq("ABC", "axc", "nonfatal context");
        break;
      case 3:
        expect_strcasene("SAME", "same", "nonfatal context");
        break;
      case 4:
        assert_streq(L"caféΩ😀", L"caféX😀", "fatal context");
        break;
      case 5:
        assert_strne(L"same", L"same", "fatal context");
        break;
      case 6:
        assert_strcaseeq(L"ABC", L"axc", "fatal context");
        break;
      case 7:
        assert_strcasene(L"SAME", L"same", "fatal context");
        break;
    }
    std::println("continued");
  }
};
int main(int argc, char** argv) { return run_tests<DiagnosticTests>(argc, argv); }
