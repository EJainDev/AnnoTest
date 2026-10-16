import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= test{}]] void same() {}
  [[= test{}]] void same(test_context&) {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
