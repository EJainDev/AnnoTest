import annotest;
import std;
constexpr std::string empty(std::string_view) { return ""; }
struct [[= annotest::name_generator<&empty>{}]] Generated {
  [[= annotest::test{}]] void body() {}
};
int main(int argc, char** argv) { return annotest::run_tests<Generated>(argc, argv); }
