import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= test{}]] void body() = delete;
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
