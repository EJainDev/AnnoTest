import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= test{}]] void body(int) {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
