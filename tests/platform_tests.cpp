#include <cstdlib>
#include <cstring>
#include <iostream>

#include "annotest_platform.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#ifdef __linux__
namespace {
bool fail_symbols = false;
}
extern "C" char** __real_backtrace_symbols(void* const*, int);
extern "C" char** __wrap_backtrace_symbols(void* const* frames, int size) {
  return fail_symbols ? nullptr : __real_backtrace_symbols(frames, size);
}
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
  const auto arguments = annotest_platform::utf8_arguments(argc, argv);
  if (arguments.size() != static_cast<std::size_t>(argc)) return 12;
  if (argc == 2 && arguments[1] != "--utf8=café") return 13;
  unsigned target = 0;
  std::uintptr_t ready = 0;
  bool normal = false, missing = false, crash = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    constexpr auto prefix = "--annotest-death-child=";
    if (arg.starts_with(prefix)) {
      auto colon = arg.find(':');
      target = std::stoul(arg.substr(std::strlen(prefix), colon));
      ready = std::stoull(arg.substr(colon + 1));
    }
    normal |= arg == "--annotest-death-name=6e6f726d616c";
    missing |= arg == "--annotest-death-name=6d697373696e67";
    crash |= arg == "--annotest-death-name=61626f7274";
  }
  if (target) {
    if (missing) return 9;  // Startup failure must not count as callable death.
    annotest_platform::configure_death_child(target, ready);
    for (unsigned i = 1; i <= target; ++i) {
      if (annotest_platform::enter_death_child()) {
        if (crash) std::abort();
        std::_Exit(normal ? 0 : 7);
      }
    }
    return 0;
  }
#else
  (void)argc;
  (void)argv;
#endif
  using namespace annotest_platform;
  auto* stream = temporary_file();
  if (!stream) return 1;
  const int saved = duplicate(1);
  if (saved < 0 || redirect(descriptor(stream), 1) < 0) return 2;
  sync_standard_handles();
  std::fputs("CRT output\n", stdout);
  std::fflush(stdout);
#ifdef _WIN32
  DWORD written = 0;
  if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "native output\n", 14, &written, nullptr) ||
      written != 14)
    return 3;
#endif
  if (redirect(saved, 1) < 0 || close_descriptor(saved) < 0) return 4;
  sync_standard_handles();
  std::rewind(stream);
  std::string output;
  char buffer[256];
  while (auto count = std::fread(buffer, 1, sizeof(buffer), stream)) output.append(buffer, count);
  std::fclose(stream);
#ifdef _WIN32
  if (output != "CRT output\nnative output\n") return 5;
#else
  if (output != "CRT output\n") return 5;
#endif
  if (capture_stacktrace().empty()) return 6;
#ifdef __linux__
  fail_symbols = true;
  if (!capture_stacktrace().empty()) return 11;
  fail_symbols = false;
#endif
#ifdef _WIN32
  reset_death_ordinal();
  if (!spawn_death_child("quoted \"name\" café\\").died) return 7;
  if (!spawn_death_child("second").died) return 8;
  if (!spawn_death_child("abort").died) return 14;
  auto normal_result = spawn_death_child("normal");
  if (normal_result.died || normal_result.detail != "exited normally with status 0") return 9;
  auto missing_result = spawn_death_child("missing");
  if (missing_result.died || missing_result.detail != "Child did not reach the death-test callable")
    return 10;
#endif
  std::cout << "Native output capture, stack traces, and process backend passed\n";
}
