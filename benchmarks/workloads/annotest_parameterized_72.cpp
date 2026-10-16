import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct bench_0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(0), parameters(3), parameters(6), parameters(9), parameters(12), parameters(15), parameters(18), parameters(21), parameters(24), parameters(27), parameters(30), parameters(33), parameters(36), parameters(39), parameters(42), parameters(45), parameters(48), parameters(51), parameters(54), parameters(57), parameters(60), parameters(63), parameters(66), parameters(69)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(1), parameters(4), parameters(7), parameters(10), parameters(13), parameters(16), parameters(19), parameters(22), parameters(25), parameters(28), parameters(31), parameters(34), parameters(37), parameters(40), parameters(43), parameters(46), parameters(49), parameters(52), parameters(55), parameters(58), parameters(61), parameters(64), parameters(67), parameters(70)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
struct bench_2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}, =parameterize{parameters(2), parameters(5), parameters(8), parameters(11), parameters(14), parameters(17), parameters(20), parameters(23), parameters(26), parameters(29), parameters(32), parameters(35), parameters(38), parameters(41), parameters(44), parameters(47), parameters(50), parameters(53), parameters(56), parameters(59), parameters(62), parameters(65), parameters(68), parameters(71)}]] void data(int input) {
    ++executed; assert_equal(input + 1, input + value);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<bench_0>(argc, argv);
  status |= run_tests<bench_1>(argc, argv);
  status |= run_tests<bench_2>(argc, argv);
  return status || executed != 72;
}
