import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= after_each{}]] void first() {}
  [[= after_each{}]] void second() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
