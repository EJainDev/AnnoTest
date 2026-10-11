import annotest;
using namespace annotest;

struct InvalidSuite {
 protected:
  [[= before_each{}]] void setup() {}
};

int main(int argc, char** argv) { return run_tests<InvalidSuite>(argc, argv); }
