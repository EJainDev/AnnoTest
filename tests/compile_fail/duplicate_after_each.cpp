import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= after_each{}, = after_each{}]] void first() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
