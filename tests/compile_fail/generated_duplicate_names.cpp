import annotest;
import std;
constexpr std::string duplicate(std::string_view) { return "same"; }
struct [[= annotest::name_generator<&duplicate>{}]] Generated {
  [[= annotest::test{}]] void first() {}
  [[= annotest::test{}]] void second() {}
};
int main(int argc, char** argv) { return annotest::run_tests<Generated>(argc, argv); }
