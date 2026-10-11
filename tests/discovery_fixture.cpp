#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <thread>

#ifdef _WIN32
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
  // Emit the exact protocol bytes, including the explicit CRLF scenario.
  _setmode(_fileno(stdout), _O_BINARY);
#endif
  using namespace std::literals;
  if (argc == 2 && std::string_view(argv[1]) == "--list") {
    const auto* raw = std::getenv("ANNOTEST_DISCOVERY_SCENARIO");
    auto mode = raw ? std::string_view(raw) : "valid"sv;
    if (mode == "failed") {
      std::cout << "Fixture.\n\tpartial\n";
      std::cerr << "deliberate listing failure\n";
      return 9;
    }
    if (mode == "timeout") {
      std::this_thread::sleep_for(5s);
      return 0;
    }
    if (mode == "empty") return 0;
    if (mode == "suite_only")
      std::cout << "Fixture.\n";
    else if (mode == "malformed")
      std::cout << "unexpected diagnostics\n";
    else if (mode == "orphan")
      std::cout << "\torphan\n";
    else if (mode == "duplicate")
      std::cout << "Fixture.\n\tok\n\tok\n";
    else if (mode == "crlf")
      std::cout << "Fixture.\r\n\tok\r\n";
    else if (mode == "special")
      std::cout << "Fixture.\n\tquote \"name\"; $ [=[ ]=] \\ ending.\n\t  padded  \n";
    else
      std::cout << "Fixture.\n\tok\n";
    return 0;
  }
  if (argc == 3 && std::string_view(argv[1]) == "--test-name") {
    auto name = std::string_view(argv[2]);
    return name == "Fixture.ok" || name == "Fixture.quote \"name\"; $ [=[ ]=] \\ ending." ||
                   name == "Fixture.  padded  "
               ? 0
               : 4;
  }
  return 2;
}
