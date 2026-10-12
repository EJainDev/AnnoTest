import annotest;
using namespace annotest;
struct invalid_suite {
 private:
  [[= before_all{}]] static void hook() {}
};
int main(int argc, char** argv) { return run_tests<invalid_suite>(argc, argv); }
