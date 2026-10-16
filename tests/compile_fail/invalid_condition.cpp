import annotest;
using namespace annotest;
void invalid() {}
struct Invalid {
  [[ = test{}, = enabled_if<&invalid>{} ]] void body() {}
};
int main(int argc, char** argv) { return run_tests<Invalid>(argc, argv); }
