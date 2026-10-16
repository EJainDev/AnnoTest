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
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 24;
}
