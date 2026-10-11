import annotest;
import std;

using namespace annotest;

#if defined(__linux__)
constexpr auto current_os = OS::Linux;
#elif defined(__APPLE__)
constexpr auto current_os = OS::Mac;
#elif defined(_WIN32)
constexpr auto current_os = OS::Windows;
#else
constexpr auto current_os = OS::Unknown;
#endif

static_assert(test{}.is_empty());
static_assert(!test<"name">{}.is_empty());
static_assert(suite{}.is_empty());
static_assert(!suite<"suite name">{}.is_empty());
static_assert(test{.disabled = true}.disabled);
static_assert(suite{.disabled = true}.disabled);

struct UnrelatedAnnotation {};

template <typename T = void>
struct UnrelatedTemplateAnnotation {};

struct[[= suite{}]] AnnotationTests {
  [[= test{}]] void implicit_name() { std::println("implicit"); }
  [[= test<"custom name">{}]] void original_name() { std::println("custom"); }
  [[= test{}]] void defaults(int n = 42) { assert_equal(42, n); }
  [[= test{}]] static void static_method() { std::println("static"); }
  [[= test{}]] void const_method() const { std::println("const"); }
  [[= test{.disabled = true}]] void disabled() { assert_true(false); }
  [[ = test{}, = requires_os{current_os} ]] void required_current() { std::println("allowed"); }
  [[ = test{},
     = requires_os{OS::Windows, OS::Mac, OS::Linux, OS::Unknown} ]] void required_multiple() {
    std::println("allowed");
  }
  [[ = test{}, = requires_os{OS::Unknown} ]] void required_other() { assert_true(false); }
  [[ = test{}, = disallows_os{current_os} ]] void disallowed_current() { assert_true(false); }
  [[ = test{},
     = disallows_os{OS::Windows, OS::Mac, OS::Linux, OS::Unknown} ]] void disallowed_multiple() {
    assert_true(false);
  }
  [[ = test{}, = disallows_os{OS::Unknown} ]] void disallowed_other() { std::println("allowed"); }
  [[ = test{}, = requires_os{current_os}, = disallows_os{current_os} ]] void contradictory_os() {
    assert_true(false);
  }
  [[ = test{}, = disallows_os{current_os},
     = parameterize{parameters(1), parameters(2)} ]] void os_values(int) {
    assert_true(false);
  }
  [[ = test{}, = requires_os{OS::Unknown},
     = parameterize{parameters(1), parameters(2)} ]] void required_values(int) {
    assert_true(false);
  }
  [[ = UnrelatedAnnotation{}, = UnrelatedTemplateAnnotation{},
     = test{} ]] void unrelated_before_test() {
    std::println("unrelated before");
  }
  [[ = test{}, = UnrelatedTemplateAnnotation{},
     = UnrelatedAnnotation{} ]] void unrelated_after_test() {
    std::println("unrelated after");
  }
  [[= UnrelatedTemplateAnnotation{}]] void unrelated_without_test() { assert_true(false); }
  [[= UnrelatedAnnotation{}]] void unrelated_plain_without_test() { assert_true(false); }
  template <typename T>
  void public_template_helper(T) {
    assert_true(false);
  }
  template <typename T = void>
  [[= UnrelatedAnnotation{}]] void unrelated_template_helper() {
    assert_true(false);
  }
  template <typename T>
  static constexpr bool variable_template_helper = sizeof(T) > 0;
  void unannotated() { assert_true(false); }
  [[= requires_os{current_os}]] void os_without_test() { assert_true(false); }

 private:
  template <typename T>
  void private_template_helper(T) {
    assert_true(false);
  }
  void inaccessible() { assert_true(false); }
};

struct[[ = UnrelatedAnnotation{}, = UnrelatedTemplateAnnotation{}, = suite<"renamed suite">{} ]]
    NamedSuite {
  [[= test{}]] void body() { std::println("named suite"); }
};
struct[[= suite{.disabled = true}]] DisabledSuite {
  [[= before_each{}]] void setup() { assert_true(false); }
  [[= after_each{}]] void cleanup() { assert_true(false); }
  [[= test{}]] void body() { assert_true(false); }
  [[ = test{}, = parameterize{parameters(1), parameters(2)} ]] void values(int) {
    assert_true(false);
  }
};
struct EmptySuite {};

int main(int argc, char** argv) {
  auto mode = std::getenv("ANNOTEST_SCENARIO");
  if (mode && std::string_view(mode) == "named") return run_tests<NamedSuite>(argc, argv);
  if (mode && std::string_view(mode) == "disabled") return run_tests<DisabledSuite>(argc, argv);
  if (mode && std::string_view(mode) == "empty") return run_tests<EmptySuite>(argc, argv);
  return run_tests<AnnotationTests>(argc, argv);
}
