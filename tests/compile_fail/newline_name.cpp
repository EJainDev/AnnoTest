import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= test<"line\nbreak">{}]] void body() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
