#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

#include "annotest_platform.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
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
  std::string target_path;
  std::uintptr_t ready = 0;
  bool normal = false, missing = false, crash = false, nested = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    constexpr auto prefix = "--annotest-death-child=";
    if (arg.starts_with(prefix)) {
      auto colon = arg.find(':');
      target_path = arg.substr(std::strlen(prefix), colon - std::strlen(prefix));
      target = std::stoul(target_path);
      ready = std::stoull(arg.substr(colon + 1));
    }
    nested |= arg == "--annotest-death-name=6e6573746564";
    normal |= arg == "--annotest-death-name=6e6f726d616c";
    missing |= arg == "--annotest-death-name=6d697373696e67";
    crash |= arg == "--annotest-death-name=61626f7274";
  }
  if (target) {
    if (missing) return 9;  // Startup failure must not count as callable death.
    if (!annotest_platform::configure_death_child(target_path, ready)) return 17;
    if (nested) {
      std::function<bool(unsigned)> check = [&](unsigned depth) {
        if (annotest_platform::death_child_active() &&
            !annotest_platform::death_callable_active()) {
          if (!annotest_platform::enter_death_child()) return true;
          if (depth < 3 && !check(depth + 1)) std::_Exit(0);
          std::_Exit(7);
        }
        return annotest_platform::spawn_death_child("nested").died;
      };
      for (unsigned i = 1; i <= target; ++i) check(1);
      return 0;
    }
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
#ifdef _WIN32
  auto* native_output = temporary_file();
  auto* native_error = temporary_file();
  if (!native_output || !native_error) return 15;
  const auto original_handles = save_standard_handles();
  // Force the CRT's closed original handle to be reused while capture is active.
  // Native restoration must follow the restored CRT descriptor, not that number.
  auto* alias_sink = temporary_file();
  if (!alias_sink) return 20;
  const int alias_original = duplicate(1);
  if (alias_original < 0 || redirect(descriptor(alias_sink), 1) < 0) return 21;
  sync_standard_handles();
  const auto alias_handles = save_standard_handles();
  auto* alias_capture = temporary_file();
  const int alias_saved = duplicate(1);
  if (!alias_capture || alias_saved < 0 || redirect(descriptor(alias_capture), 1) < 0) return 22;
  sync_standard_handles();
  auto* occupied_handle = temporary_file();
  if (!occupied_handle || redirect(alias_saved, 1) < 0 || close_descriptor(alias_saved) < 0)
    return 23;
  restore_standard_handles(alias_handles);
  DWORD alias_written = 0;
  if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "restored native", 15, &alias_written, nullptr) ||
      alias_written != 15)
    return 24;
  std::rewind(alias_sink);
  char alias_buffer[16]{};
  if (std::fread(alias_buffer, 1, 15, alias_sink) != 15 ||
      std::string_view(alias_buffer, 15) != "restored native")
    return 25;
  if (redirect(alias_original, 1) < 0 || close_descriptor(alias_original) < 0) return 26;
  restore_standard_handles(original_handles);
  std::fclose(alias_capture);
  std::fclose(occupied_handle);
  std::fclose(alias_sink);
  SetStdHandle(STD_OUTPUT_HANDLE,
               reinterpret_cast<HANDLE>(_get_osfhandle(descriptor(native_output))));
  SetStdHandle(STD_ERROR_HANDLE,
               reinterpret_cast<HANDLE>(_get_osfhandle(descriptor(native_error))));
#endif
  const auto native_saved = save_standard_handles();
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
  restore_standard_handles(native_saved);
#ifdef _WIN32
  const auto restored = save_standard_handles();
  if (restored.output != native_saved.output || restored.error != native_saved.error) return 16;
  restore_standard_handles(original_handles);
  std::fclose(native_output);
  std::fclose(native_error);
#endif
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
  for (auto path : {"", "0", "1.", ".1", "1..2", "1.a", "4294967296"})
    if (configure_death_child(path, 1)) return 19;
  reset_death_ordinal();
  if (!spawn_death_child("quoted \"name\" café\\").died) return 7;
  if (!spawn_death_child("second").died) return 8;
  if (!spawn_death_child("abort").died) return 14;
  if (!spawn_death_child("nested").died) return 18;
  auto normal_result = spawn_death_child("normal");
  if (normal_result.died || normal_result.detail != "exited normally with status 0") return 9;
  auto missing_result = spawn_death_child("missing");
  if (missing_result.died || missing_result.detail != "Child did not reach the death-test callable")
    return 10;
#endif
  std::cout << "Native output capture, stack traces, and process backend passed\n";
}
