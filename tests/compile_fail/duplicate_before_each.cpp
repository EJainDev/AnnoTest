import annotest;
using namespace annotest;

struct InvalidSuite {
  [[= before_each{}, = before_each{}]] void first() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
