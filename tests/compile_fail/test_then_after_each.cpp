import annotest;
using namespace annotest;

struct InvalidSuite {
  [[ = test{}, = after_each{} ]] void mixed() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
