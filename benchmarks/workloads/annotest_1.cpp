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
};
int main(int argc, char** argv) {
  int status = 0;
  status |= run_tests<Bench0>(argc, argv);
  return status || executed != 1;
}
