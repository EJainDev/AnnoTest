import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct bench_0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(0), parameters(3), parameters(6), parameters(9), parameters(12), parameters(15), parameters(18), parameters(21)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(1), parameters(4), parameters(7), parameters(10), parameters(13), parameters(16), parameters(19), parameters(22)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(2), parameters(5), parameters(8), parameters(11), parameters(14), parameters(17), parameters(20), parameters(23)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<bench_0>(argc, argv);
  status |= run_tests<bench_1>(argc, argv);
  status |= run_tests<bench_2>(argc, argv);
  return status || executed != 24;
}
