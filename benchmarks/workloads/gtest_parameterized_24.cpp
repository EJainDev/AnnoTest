#include <gtest/gtest.h>
static int executed = 0;
class bench_0 : public testing::TestWithParam<int> {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_P(bench_0, data) { ++executed; ASSERT_EQ(GetParam() + 1, GetParam() + value); }
INSTANTIATE_TEST_SUITE_P(inputs, bench_0, testing::Values(0, 3, 6, 9, 12, 15, 18, 21));
class bench_1 : public testing::TestWithParam<int> {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_P(bench_1, data) { ++executed; ASSERT_EQ(GetParam() + 1, GetParam() + value); }
INSTANTIATE_TEST_SUITE_P(inputs, bench_1, testing::Values(1, 4, 7, 10, 13, 16, 19, 22));
class bench_2 : public testing::TestWithParam<int> {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_P(bench_2, data) { ++executed; ASSERT_EQ(GetParam() + 1, GetParam() + value); }
INSTANTIATE_TEST_SUITE_P(inputs, bench_2, testing::Values(2, 5, 8, 11, 14, 17, 20, 23));
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 24;
}
