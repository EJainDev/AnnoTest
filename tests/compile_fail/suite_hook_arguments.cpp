import annotest;
using namespace annotest;
struct invalid_suite {
  [[= after_all{}]] static void hook(int) {}
};
int main(int argc, char** argv) { return run_tests<invalid_suite>(argc, argv); }
