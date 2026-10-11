import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= test<"same">{}]] void first() {}
  [[= test<"same">{}]] void second() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
