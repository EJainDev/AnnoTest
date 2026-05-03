import cpputils.testing;

using namespace cpputils::testing;

import std;

struct TestSuite {
  [[= BeforeEach{}]] void setUp() { std::cout << "Running set up...\n"; }
  [[= Test{}]] void test1() { assertEqual(3, 3); }
  [[ = Test{}, = Disabled{} ]] void disabledTest() { std::cout << "Never executes\n"; }
  [[= AfterEach{}]] void cleanUp() { std::cout << "Running clean up...\n"; }
};

int main(int argc, char** argv) {
  return test<TestSuite>(argc, argv);  // Call the testing suite
}