import annotest;
import std;

using namespace annotest;
enum class Colour { red = -1, blue = 3, green = 10 };
enum Plain { first, second };

struct UtilityTests {
  [[= test<"quoted \"name\"; $ [=[ ]=] and trailing.">{}]] void special_name() {
    assert_true(true);
  }
  [[= test<"named enum">{}]] void named() {
    assert_equal(std::string{"linux_os"}, enum_to_string(operating_system::linux_os));
  }
  [[
    = test{}, = parameterize{parameters(operating_system::windows), parameters(operating_system::macos), parameters(operating_system::linux_os)}
  ]] void os_values(operating_system os) {
    assert_not_equal(std::string{"unknown"}, enum_to_string(os));
  }

  [[= test{}]] void enums() {
    assert_equal(std::string{"red"}, enum_to_string(Colour::red));
    assert_equal(std::string{"blue"}, enum_to_string(Colour::blue));
    assert_equal(std::string{"green"}, enum_to_string(Colour::green));
    assert_equal(std::string{"<invalid enum value>"}, enum_to_string(static_cast<Colour>(99)));
    assert_equal(std::string{"first"}, enum_to_string(first));
    assert_equal(std::string{"second"}, enum_to_string(second));
    assert_equal(std::string{"windows"}, enum_to_string(operating_system::windows));
    assert_equal(std::string{"macos"}, enum_to_string(operating_system::macos));
    assert_equal(std::string{"linux_os"}, enum_to_string(operating_system::linux_os));
    assert_equal(std::string{"unknown"}, enum_to_string(operating_system::unknown));
    assert_equal(std::string{"<invalid enum value>"}, enum_to_string(static_cast<operating_system>(99)));
  }
};
int main(int argc, char** argv) { return run_tests<UtilityTests>(argc, argv); }
