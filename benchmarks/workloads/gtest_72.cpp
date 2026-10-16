#include <gtest/gtest.h>
#include <iostream>
#include <string>
static int executed = 0;
class Bench0 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench0, Case0) {
  ++executed; ASSERT_EQ(value + 0, 1);
}
TEST_F(Bench0, Case3) {
  ++executed; ASSERT_EQ(value + 3, 4);
}
TEST_F(Bench0, Case6) {
  ++executed; ASSERT_EQ(value + 6, 7);
}
TEST_F(Bench0, Case9) {
  ++executed; ASSERT_EQ(value + 9, 10);
}
TEST_F(Bench0, Case12) {
  ++executed; ASSERT_EQ(value + 12, 13);
}
TEST_F(Bench0, Case15) {
  ++executed; ASSERT_EQ(value + 15, 16);
}
TEST_F(Bench0, Case18) {
  ++executed; ASSERT_EQ(value + 18, 19);
}
TEST_F(Bench0, Case21) {
  ++executed; ASSERT_EQ(value + 21, 22);
}
TEST_F(Bench0, Case24) {
  ++executed; ASSERT_EQ(value + 24, 25);
}
TEST_F(Bench0, Case27) {
  ++executed; ASSERT_EQ(value + 27, 28);
}
TEST_F(Bench0, Case30) {
  ++executed; ASSERT_EQ(value + 30, 31);
}
TEST_F(Bench0, Case33) {
  ++executed; ASSERT_EQ(value + 33, 34);
}
TEST_F(Bench0, Case36) {
  ++executed; ASSERT_EQ(value + 36, 37);
}
TEST_F(Bench0, Case39) {
  ++executed; ASSERT_EQ(value + 39, 40);
}
TEST_F(Bench0, Case42) {
  ++executed; ASSERT_EQ(value + 42, 43);
}
TEST_F(Bench0, Case45) {
  ++executed; ASSERT_EQ(value + 45, 46);
}
TEST_F(Bench0, Case48) {
  ++executed; ASSERT_EQ(value + 48, 49);
}
TEST_F(Bench0, Case51) {
  ++executed; ASSERT_EQ(value + 51, 52);
}
TEST_F(Bench0, Case54) {
  ++executed; ASSERT_EQ(value + 54, 55);
}
TEST_F(Bench0, Case57) {
  ++executed; ASSERT_EQ(value + 57, 58);
}
TEST_F(Bench0, Case60) {
  ++executed; ASSERT_EQ(value + 60, 61);
}
TEST_F(Bench0, Case63) {
  ++executed; ASSERT_EQ(value + 63, 64);
}
TEST_F(Bench0, Case66) {
  ++executed; ASSERT_EQ(value + 66, 67);
}
TEST_F(Bench0, Case69) {
  ++executed; ASSERT_EQ(value + 69, 70);
}
class Bench1 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench1, Case1) {
  ++executed; ASSERT_EQ(value + 1, 2);
}
TEST_F(Bench1, Case4) {
  ++executed; ASSERT_EQ(value + 4, 5);
}
TEST_F(Bench1, Case7) {
  ++executed; ASSERT_EQ(value + 7, 8);
}
TEST_F(Bench1, Case10) {
  ++executed; ASSERT_EQ(value + 10, 11);
}
TEST_F(Bench1, Case13) {
  ++executed; ASSERT_EQ(value + 13, 14);
}
TEST_F(Bench1, Case16) {
  ++executed; ASSERT_EQ(value + 16, 17);
}
TEST_F(Bench1, Case19) {
  ++executed; ASSERT_EQ(value + 19, 20);
}
TEST_F(Bench1, Case22) {
  ++executed; ASSERT_EQ(value + 22, 23);
}
TEST_F(Bench1, Case25) {
  ++executed; ASSERT_EQ(value + 25, 26);
}
TEST_F(Bench1, Case28) {
  ++executed; ASSERT_EQ(value + 28, 29);
}
TEST_F(Bench1, Case31) {
  ++executed; ASSERT_EQ(value + 31, 32);
}
TEST_F(Bench1, Case34) {
  ++executed; ASSERT_EQ(value + 34, 35);
}
TEST_F(Bench1, Case37) {
  ++executed; ASSERT_EQ(value + 37, 38);
}
TEST_F(Bench1, Case40) {
  ++executed; ASSERT_EQ(value + 40, 41);
}
TEST_F(Bench1, Case43) {
  ++executed; ASSERT_EQ(value + 43, 44);
}
TEST_F(Bench1, Case46) {
  ++executed; ASSERT_EQ(value + 46, 47);
}
TEST_F(Bench1, Case49) {
  ++executed; ASSERT_EQ(value + 49, 50);
}
TEST_F(Bench1, Case52) {
  ++executed; ASSERT_EQ(value + 52, 53);
}
TEST_F(Bench1, Case55) {
  ++executed; ASSERT_EQ(value + 55, 56);
}
TEST_F(Bench1, Case58) {
  ++executed; ASSERT_EQ(value + 58, 59);
}
TEST_F(Bench1, Case61) {
  ++executed; ASSERT_EQ(value + 61, 62);
}
TEST_F(Bench1, Case64) {
  ++executed; ASSERT_EQ(value + 64, 65);
}
TEST_F(Bench1, Case67) {
  ++executed; ASSERT_EQ(value + 67, 68);
}
TEST_F(Bench1, Case70) {
  ++executed; ASSERT_EQ(value + 70, 71);
}
class Bench2 : public testing::Test {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_F(Bench2, Case2) {
  ++executed; ASSERT_EQ(value + 2, 3);
}
TEST_F(Bench2, Case5) {
  ++executed; ASSERT_EQ(value + 5, 6);
}
TEST_F(Bench2, Case8) {
  ++executed; ASSERT_EQ(value + 8, 9);
}
TEST_F(Bench2, Case11) {
  ++executed; ASSERT_EQ(value + 11, 12);
}
TEST_F(Bench2, Case14) {
  ++executed; ASSERT_EQ(value + 14, 15);
}
TEST_F(Bench2, Case17) {
  ++executed; ASSERT_EQ(value + 17, 18);
}
TEST_F(Bench2, Case20) {
  ++executed; ASSERT_EQ(value + 20, 21);
}
TEST_F(Bench2, Case23) {
  ++executed; ASSERT_EQ(value + 23, 24);
}
TEST_F(Bench2, Case26) {
  ++executed; ASSERT_EQ(value + 26, 27);
}
TEST_F(Bench2, Case29) {
  ++executed; ASSERT_EQ(value + 29, 30);
}
TEST_F(Bench2, Case32) {
  ++executed; ASSERT_EQ(value + 32, 33);
}
TEST_F(Bench2, Case35) {
  ++executed; ASSERT_EQ(value + 35, 36);
}
TEST_F(Bench2, Case38) {
  ++executed; ASSERT_EQ(value + 38, 39);
}
TEST_F(Bench2, Case41) {
  ++executed; ASSERT_EQ(value + 41, 42);
}
TEST_F(Bench2, Case44) {
  ++executed; ASSERT_EQ(value + 44, 45);
}
TEST_F(Bench2, Case47) {
  ++executed; ASSERT_EQ(value + 47, 48);
}
TEST_F(Bench2, Case50) {
  ++executed; ASSERT_EQ(value + 50, 51);
}
TEST_F(Bench2, Case53) {
  ++executed; ASSERT_EQ(value + 53, 54);
}
TEST_F(Bench2, Case56) {
  ++executed; ASSERT_EQ(value + 56, 57);
}
TEST_F(Bench2, Case59) {
  ++executed; ASSERT_EQ(value + 59, 60);
}
TEST_F(Bench2, Case62) {
  ++executed; ASSERT_EQ(value + 62, 63);
}
TEST_F(Bench2, Case65) {
  ++executed; ASSERT_EQ(value + 65, 66);
}
TEST_F(Bench2, Case68) {
  ++executed; ASSERT_EQ(value + 68, 69);
}
TEST_F(Bench2, Case71) {
  ++executed; ASSERT_EQ(value + 71, 72);
}
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 72;
}
