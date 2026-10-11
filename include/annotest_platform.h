#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

namespace annotest_platform {
std::FILE* temporary_file();
int duplicate(int descriptor);
int redirect(int source, int destination);
int close_descriptor(int descriptor);
int descriptor(std::FILE* stream);
void sync_standard_handles();
std::string capture_stacktrace();
std::vector<std::string> utf8_arguments(int argc, char** argv);

struct death_result {
  bool died;
  std::string detail;
};
// Internal re-execution protocol, used only on Windows.
void configure_death_child(unsigned ordinal, std::uintptr_t ready_handle);
bool death_child_active();
void reset_death_ordinal();
bool enter_death_child();
death_result spawn_death_child(std::string_view name);
}  // namespace annotest_platform
