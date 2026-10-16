import annotest;
using namespace annotest;

struct InvalidSuite {
 private:
  [[= test{}]] void body() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
