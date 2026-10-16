import annotest;
using namespace annotest;

struct InvalidSuite {
  [[ = test<"value">{}, = parameterize{parameters(1)} ]] void values(int) {}
  [[= test<"value.1">{}]] void literal_case() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
