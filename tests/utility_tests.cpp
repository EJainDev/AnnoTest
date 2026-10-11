import annotest;
import std;

using namespace annotest;
enum class Colour { red = -1, blue = 3, green = 10 };
enum Plain { first, second };

struct UtilityTests {
  [[= test<"named enum">{}]] void named() {
    assert_equal(std::string{"Linux"}, enum_to_string(OS::Linux));
  }
  [[
    = test{}, = parameterize{parameters(OS::Windows), parameters(OS::Mac), parameters(OS::Linux)}
  ]] void os_values(OS os) {
    assert_not_equal(std::string{"Unknown"}, enum_to_string(os));
  }

  [[= test{}]] void enums() {
    assert_equal(std::string{"red"}, enum_to_string(Colour::red));
    assert_equal(std::string{"blue"}, enum_to_string(Colour::blue));
    assert_equal(std::string{"green"}, enum_to_string(Colour::green));
    assert_equal(std::string{"<invalid enum value>"}, enum_to_string(static_cast<Colour>(99)));
    assert_equal(std::string{"first"}, enum_to_string(first));
    assert_equal(std::string{"second"}, enum_to_string(second));
    assert_equal(std::string{"Windows"}, enum_to_string(OS::Windows));
    assert_equal(std::string{"Mac"}, enum_to_string(OS::Mac));
    assert_equal(std::string{"Linux"}, enum_to_string(OS::Linux));
    assert_equal(std::string{"Unknown"}, enum_to_string(OS::Unknown));
    assert_equal(std::string{"<invalid enum value>"}, enum_to_string(static_cast<OS>(99)));
  }
};
int main(int argc, char** argv) { return run_tests<UtilityTests>(argc, argv); }
