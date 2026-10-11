import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= before_each{}]] void first() {}
  [[= before_each{}]] void second() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
