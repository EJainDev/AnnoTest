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
    std::cout << std::string(1024 * 1024, 'x');
}
int main(int argc, char** argv) {
  int status = 0;
  testing::InitGoogleTest(&argc, argv);
  status = RUN_ALL_TESTS();
  return status || executed != 1;
}
