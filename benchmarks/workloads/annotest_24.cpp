import annotest;
import std;
using namespace annotest;
static int executed = 0;
struct Bench0 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_0() {
    ++executed; assert_equal(value + 0, 1);
  }
  [[=test{}]] void case_3() {
    ++executed; assert_equal(value + 3, 4);
  }
  [[=test{}]] void case_6() {
    ++executed; assert_equal(value + 6, 7);
  }
  [[=test{}]] void case_9() {
    ++executed; assert_equal(value + 9, 10);
  }
  [[=test{}]] void case_12() {
    ++executed; assert_equal(value + 12, 13);
  }
  [[=test{}]] void case_15() {
    ++executed; assert_equal(value + 15, 16);
  }
  [[=test{}]] void case_18() {
    ++executed; assert_equal(value + 18, 19);
  }
  [[=test{}]] void case_21() {
    ++executed; assert_equal(value + 21, 22);
  }
};
struct Bench1 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_1() {
    ++executed; assert_equal(value + 1, 2);
  }
  [[=test{}]] void case_4() {
    ++executed; assert_equal(value + 4, 5);
  }
  [[=test{}]] void case_7() {
    ++executed; assert_equal(value + 7, 8);
  }
  [[=test{}]] void case_10() {
    ++executed; assert_equal(value + 10, 11);
  }
  [[=test{}]] void case_13() {
    ++executed; assert_equal(value + 13, 14);
  }
  [[=test{}]] void case_16() {
    ++executed; assert_equal(value + 16, 17);
  }
  [[=test{}]] void case_19() {
    ++executed; assert_equal(value + 19, 20);
  }
  [[=test{}]] void case_22() {
    ++executed; assert_equal(value + 22, 23);
  }
};
struct Bench2 {
  int value = 0;
  [[=before_each{}]] void setup() { value = 1; }
  [[=after_each{}]] void teardown() { value = 0; }
  [[=test{}]] void case_2() {
    ++executed; assert_equal(value + 2, 3);
  }
  [[=test{}]] void case_5() {
    ++executed; assert_equal(value + 5, 6);
  }
  [[=test{}]] void case_8() {
    ++executed; assert_equal(value + 8, 9);
  }
  [[=test{}]] void case_11() {
    ++executed; assert_equal(value + 11, 12);
  }
  [[=test{}]] void case_14() {
    ++executed; assert_equal(value + 14, 15);
  }
  [[=test{}]] void case_17() {
    ++executed; assert_equal(value + 17, 18);
  }
  [[=test{}]] void case_20() {
    ++executed; assert_equal(value + 20, 21);
  }
  [[=test{}]] void case_23() {
    ++executed; assert_equal(value + 23, 24);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<Bench0>(argc, argv);
  status |= run_tests<Bench1>(argc, argv);
  status |= run_tests<Bench2>(argc, argv);
  return status || executed != 24;
}
