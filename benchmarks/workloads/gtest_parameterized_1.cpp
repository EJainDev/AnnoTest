#include <gtest/gtest.h>
static int executed = 0;
class bench_0 : public testing::TestWithParam<int> {
 protected:
  int value = 0;
  void SetUp() override { value = 1; }
  void TearDown() override { value = 0; }
};
TEST_P(bench_0, data) { ++executed; ASSERT_EQ(GetParam() + 1, GetParam() + value); }
INSTANTIATE_TEST_SUITE_P(inputs, bench_0, testing::Values(0));
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 1;
}
