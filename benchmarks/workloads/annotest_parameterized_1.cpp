import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct bench_0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(0)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<bench_0>(argc, argv);
  return status || executed != 1;
}
