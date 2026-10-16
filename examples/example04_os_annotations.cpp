// OS annotations filter test execution; they do not make the library portable.
// The runner detects Linux, macOS, and native Windows.
import annotest;
import std;

using namespace annotest;

struct PlatformTests {
  [[ = test{}, = requires_os{operating_system::linux_os} ]] void linux_only() {
    assert_equal("linux_os", enum_to_string(operating_system::linux_os));
  }

  // Multiple required platforms use OR logic.
  [[ = test{}, = requires_os{operating_system::linux_os, operating_system::macos} ]] void unix_platform() { assert_equal(5, 2 + 3); }

  [[ = test{}, = disallows_os{operating_system::windows, operating_system::unknown} ]] void known_unix_platform() {
    assert_true(std::filesystem::path("/tmp").is_absolute());
  }

  [[ = test{}, = requires_os{operating_system::windows} ]] void windows_only() {
    // This test is skipped on Linux. Its body must still compile there.
    assert_equal("windows", enum_to_string(operating_system::windows));
  }
};

int main(int argc, char** argv) { return run_tests<PlatformTests>(argc, argv); }
