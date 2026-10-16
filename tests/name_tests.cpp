import annotest;
import std;

using namespace annotest;

struct NameTests {
  [[ = test<"value">{}, = parameterize{parameters(1), parameters(2)} ]] void values(int n) {
    assert_true(n > 0);
  }
  [[= test<"value.special">{}]] void special() { assert_true(true); }
  [[= test<"value.special.99">{}]] void dotted() { assert_true(true); }
  [[ = test<"nested.value">{},
     = parameterize{parameters(1), parameters(2)} ]] void nested_values(int n) {
    assert_true(n > 0);
  }
  [[= test<"nested.value.note">{}]] void note() { assert_true(true); }
};

struct[[= suite<"-smoke">{}]] LeadingSuite {
  [[= test{}]] void body() { assert_true(true); }
};

int main(int argc, char** argv) {
  auto mode = std::getenv("ANNOTEST_SCENARIO");
  if (mode && std::string_view(mode) == "leading") return run_tests<LeadingSuite>(argc, argv);
  return run_tests<NameTests>(argc, argv);
}
