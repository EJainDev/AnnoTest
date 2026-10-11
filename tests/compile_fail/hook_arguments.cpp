import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= after_each{}]] void cleanup(int) {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
