import annotest;
using namespace annotest;

struct InvalidSuite {
  [[ = test{}, = before_each{} ]] void mixed() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
