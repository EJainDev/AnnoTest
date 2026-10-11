// OS annotations filter test execution; they do not make the library portable.
// Linux is the currently verified build platform.
import annotest;
import std;

using namespace annotest;

struct PlatformTests {
  [[ = test{}, = requires_os{OS::Linux} ]] void linux_only() {
    assert_equal("Linux", enum_to_string(OS::Linux));
  }

  // Multiple required platforms use OR logic.
  [[ = test{}, = requires_os{OS::Linux, OS::Mac} ]] void unix_platform() { assert_equal(5, 2 + 3); }

  [[ = test{}, = disallows_os{OS::Windows, OS::Unknown} ]] void known_unix_platform() {
    assert_true(std::filesystem::path("/tmp").is_absolute());
  }

  [[ = test{}, = requires_os{OS::Windows} ]] void windows_only() {
    // This test is skipped on Linux. Its body must still compile there.
    assert_equal("Windows", enum_to_string(OS::Windows));
  }
};

int main(int argc, char** argv) { return run_tests<PlatformTests>(argc, argv); }
