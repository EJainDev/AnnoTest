import annotest;
using namespace annotest;
struct Invalid {
  [[= test{.repetitions = 1000001}]] void body() {}
};
int main(int argc, char** argv) { return run_tests<Invalid>(argc, argv); }
