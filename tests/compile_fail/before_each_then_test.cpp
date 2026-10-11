import annotest;
using namespace annotest;

struct InvalidSuite {
  [[ = before_each{}, = test{} ]] void mixed() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
