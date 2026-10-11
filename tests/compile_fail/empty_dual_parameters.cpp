import annotest;
using namespace annotest;

struct InvalidSuite {
  template <typename T = int>
  [[ = test{}, = parameterize_dual{} ]] void body(T) {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
