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
  [[=test{}]] void case_24() {
    ++executed; assert_equal(value + 24, 25);
  }
  [[=test{}]] void case_27() {
    ++executed; assert_equal(value + 27, 28);
  }
  [[=test{}]] void case_30() {
    ++executed; assert_equal(value + 30, 31);
  }
  [[=test{}]] void case_33() {
    ++executed; assert_equal(value + 33, 34);
  }
  [[=test{}]] void case_36() {
    ++executed; assert_equal(value + 36, 37);
  }
  [[=test{}]] void case_39() {
    ++executed; assert_equal(value + 39, 40);
  }
  [[=test{}]] void case_42() {
    ++executed; assert_equal(value + 42, 43);
  }
  [[=test{}]] void case_45() {
    ++executed; assert_equal(value + 45, 46);
  }
  [[=test{}]] void case_48() {
    ++executed; assert_equal(value + 48, 49);
  }
  [[=test{}]] void case_51() {
    ++executed; assert_equal(value + 51, 52);
  }
  [[=test{}]] void case_54() {
    ++executed; assert_equal(value + 54, 55);
  }
  [[=test{}]] void case_57() {
    ++executed; assert_equal(value + 57, 58);
  }
  [[=test{}]] void case_60() {
    ++executed; assert_equal(value + 60, 61);
  }
  [[=test{}]] void case_63() {
    ++executed; assert_equal(value + 63, 64);
  }
  [[=test{}]] void case_66() {
    ++executed; assert_equal(value + 66, 67);
  }
  [[=test{}]] void case_69() {
    ++executed; assert_equal(value + 69, 70);
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
  [[=test{}]] void case_25() {
    ++executed; assert_equal(value + 25, 26);
  }
  [[=test{}]] void case_28() {
    ++executed; assert_equal(value + 28, 29);
  }
  [[=test{}]] void case_31() {
    ++executed; assert_equal(value + 31, 32);
  }
  [[=test{}]] void case_34() {
    ++executed; assert_equal(value + 34, 35);
  }
  [[=test{}]] void case_37() {
    ++executed; assert_equal(value + 37, 38);
  }
  [[=test{}]] void case_40() {
    ++executed; assert_equal(value + 40, 41);
  }
  [[=test{}]] void case_43() {
    ++executed; assert_equal(value + 43, 44);
  }
  [[=test{}]] void case_46() {
    ++executed; assert_equal(value + 46, 47);
  }
  [[=test{}]] void case_49() {
    ++executed; assert_equal(value + 49, 50);
  }
  [[=test{}]] void case_52() {
    ++executed; assert_equal(value + 52, 53);
  }
  [[=test{}]] void case_55() {
    ++executed; assert_equal(value + 55, 56);
  }
  [[=test{}]] void case_58() {
    ++executed; assert_equal(value + 58, 59);
  }
  [[=test{}]] void case_61() {
    ++executed; assert_equal(value + 61, 62);
  }
  [[=test{}]] void case_64() {
    ++executed; assert_equal(value + 64, 65);
  }
  [[=test{}]] void case_67() {
    ++executed; assert_equal(value + 67, 68);
  }
  [[=test{}]] void case_70() {
    ++executed; assert_equal(value + 70, 71);
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
  [[=test{}]] void case_26() {
    ++executed; assert_equal(value + 26, 27);
  }
  [[=test{}]] void case_29() {
    ++executed; assert_equal(value + 29, 30);
  }
  [[=test{}]] void case_32() {
    ++executed; assert_equal(value + 32, 33);
  }
  [[=test{}]] void case_35() {
    ++executed; assert_equal(value + 35, 36);
  }
  [[=test{}]] void case_38() {
    ++executed; assert_equal(value + 38, 39);
  }
  [[=test{}]] void case_41() {
    ++executed; assert_equal(value + 41, 42);
  }
  [[=test{}]] void case_44() {
    ++executed; assert_equal(value + 44, 45);
  }
  [[=test{}]] void case_47() {
    ++executed; assert_equal(value + 47, 48);
  }
  [[=test{}]] void case_50() {
    ++executed; assert_equal(value + 50, 51);
  }
  [[=test{}]] void case_53() {
    ++executed; assert_equal(value + 53, 54);
  }
  [[=test{}]] void case_56() {
    ++executed; assert_equal(value + 56, 57);
  }
  [[=test{}]] void case_59() {
    ++executed; assert_equal(value + 59, 60);
  }
  [[=test{}]] void case_62() {
    ++executed; assert_equal(value + 62, 63);
  }
  [[=test{}]] void case_65() {
    ++executed; assert_equal(value + 65, 66);
  }
  [[=test{}]] void case_68() {
    ++executed; assert_equal(value + 68, 69);
  }
  [[=test{}]] void case_71() {
    ++executed; assert_equal(value + 71, 72);
  }
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<Bench0>(argc, argv);
  status |= run_tests<Bench1>(argc, argv);
  status |= run_tests<Bench2>(argc, argv);
  return status || executed != 72;
}
