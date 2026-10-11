#include <charconv>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "annotest_platform.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <fcntl.h>
#include <io.h>
// Windows types must be declared before shellapi.h.
// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on
#else
#include <execinfo.h>
#include <unistd.h>
#endif

namespace annotest_platform {
#ifdef _WIN32
namespace {
struct handle {
  HANDLE value = INVALID_HANDLE_VALUE;
  ~handle() {
    if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value);
  }
};
std::string utf8(std::wstring_view value) {
  int size = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr,
                                 0, nullptr, nullptr);
  std::string result(size, '\0');
  WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), result.data(), size,
                      nullptr, nullptr);
  return result;
}
// Each selected ancestor contributes one ordinal; the last component selects
// the callable that sends the readiness handshake. Nested checks extend this path.
std::vector<unsigned> child_path;
std::vector<unsigned> ordinals{0};
bool callable_active = false;
HANDLE child_ready = nullptr;

std::wstring quote(std::wstring_view argument) {
  std::wstring result = L"\"";
  unsigned slashes = 0;
  for (wchar_t c : argument) {
    if (c == L'\\') {
      ++slashes;
      continue;
    }
    result.append(c == L'"' ? 2 * slashes + 1 : slashes, L'\\');
    result += c;
    slashes = 0;
  }
  result.append(2 * slashes, L'\\');
  return result + L'"';
}
HANDLE inherit_standard(DWORD kind, DWORD access) {
  HANDLE original = GetStdHandle(kind);
  HANDLE duplicate = INVALID_HANDLE_VALUE;
  if (original && original != INVALID_HANDLE_VALUE &&
      DuplicateHandle(GetCurrentProcess(), original, GetCurrentProcess(), &duplicate, 0, TRUE,
                      DUPLICATE_SAME_ACCESS))
    return duplicate;
  SECURITY_ATTRIBUTES attributes{sizeof(attributes), nullptr, TRUE};
  return CreateFileW(L"NUL", access, FILE_SHARE_READ | FILE_SHARE_WRITE, &attributes, OPEN_EXISTING,
                     0, nullptr);
}
}  // namespace

std::FILE* temporary_file() {
  wchar_t directory[MAX_PATH + 1], path[MAX_PATH + 1];
  DWORD length = GetTempPathW(MAX_PATH + 1, directory);
  if (!length || length > MAX_PATH || !GetTempFileNameW(directory, L"ant", 0, path)) return nullptr;
  HANDLE file = CreateFileW(
      path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    DeleteFileW(path);
    return nullptr;
  }
  int fd = _open_osfhandle(reinterpret_cast<std::intptr_t>(file), _O_BINARY | _O_RDWR);
  if (fd < 0) {
    CloseHandle(file);
    return nullptr;
  }
  auto* stream = _fdopen(fd, "w+b");
  if (!stream) _close(fd);
  return stream;
}
int duplicate(int fd) { return _dup(fd); }
int redirect(int source, int destination) { return _dup2(source, destination); }
int close_descriptor(int fd) { return _close(fd); }
int descriptor(std::FILE* stream) { return _fileno(stream); }
standard_handles save_standard_handles() {
  auto output = reinterpret_cast<std::uintptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
  auto error = reinterpret_cast<std::uintptr_t>(GetStdHandle(STD_ERROR_HANDLE));
  auto aliased_descriptor = [](std::uintptr_t value) {
    if (!value || value == static_cast<std::uintptr_t>(-1)) return -1;
    for (int fd : {1, 2}) {
      if (value == static_cast<std::uintptr_t>(_get_osfhandle(fd))) return fd;
    }
    return -1;
  };
  return {output, error, aliased_descriptor(output), aliased_descriptor(error)};
}
void restore_standard_handles(standard_handles saved) {
  // Redirecting a CRT descriptor closes its original OS handle. Resolve aliases
  // against the restored descriptors; independent native handles retain identity.
  auto restored = [](std::uintptr_t original, int fd) {
    return fd < 0 ? reinterpret_cast<HANDLE>(original)
                  : reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  };
  SetStdHandle(STD_OUTPUT_HANDLE, restored(saved.output, saved.output_descriptor));
  SetStdHandle(STD_ERROR_HANDLE, restored(saved.error, saved.error_descriptor));
}
void sync_standard_handles() {
  SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(_get_osfhandle(1)));
  SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(_get_osfhandle(2)));
}
std::string capture_stacktrace() {
  void* frames[32];
  const auto count = CaptureStackBackTrace(1, 32, frames, nullptr);
  std::string result;
  for (unsigned i = 0; i < count; ++i) {
    char line[48];
    std::snprintf(line, sizeof(line), "#%u %p\n", i, frames[i]);
    result += line;
  }
  return result;
}
std::vector<std::string> utf8_arguments(int argc, char** argv) {
  // Preserve synthetic argv arrays used by embedders and parser tests.
  if (argc != __argc || !argv) return {};
  for (int i = 0; i < argc; ++i) {
    if (!argv[i] || !__argv[i] || std::strcmp(argv[i], __argv[i]) != 0) return {};
  }
  int count = 0;
  auto** arguments = CommandLineToArgvW(GetCommandLineW(), &count);
  if (!arguments) return {};
  std::vector<std::string> result;
  if (count == argc) {
    for (int i = 0; i < count; ++i) result.emplace_back(utf8(arguments[i]));
  }
  LocalFree(arguments);
  return result;
}
void configure_death_child(unsigned target, std::uintptr_t ready) {
  configure_death_child(std::to_string(target), ready);
}
bool configure_death_child(std::string_view path, std::uintptr_t ready) {
  std::vector<unsigned> parsed;
  while (!path.empty()) {
    auto dot = path.find('.');
    auto part = path.substr(0, dot);
    unsigned value = 0;
    auto result = std::from_chars(part.data(), part.data() + part.size(), value);
    if (result.ec != std::errc{} || result.ptr != part.data() + part.size() || !value)
      return false;
    parsed.push_back(value);
    if (dot == std::string_view::npos) break;
    path.remove_prefix(dot + 1);
    if (path.empty()) return false;
  }
  if (parsed.empty() || !ready) return false;
  child_path = std::move(parsed);
  ordinals = {0};
  callable_active = false;
  child_ready = reinterpret_cast<HANDLE>(ready);
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#ifdef _UCRT
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
  return true;
}
bool death_child_active() { return !child_path.empty(); }
bool death_callable_active() { return callable_active; }
void reset_death_ordinal() { ordinals = {0}; }
bool enter_death_child() {
  const auto depth = ordinals.size() - 1;
  if (depth >= child_path.size() || ++ordinals.back() != child_path[depth]) return false;
  ordinals.push_back(0);
  if (ordinals.size() <= child_path.size()) return true;
  DWORD written = 0;
  const char marker = 'R';
  if (!WriteFile(child_ready, &marker, 1, &written, nullptr) || written != 1) std::_Exit(0);
  CloseHandle(child_ready);
  child_ready = nullptr;
  callable_active = true;
  return true;
}
death_result spawn_death_child(std::string_view name) {
  ++ordinals.back();
  std::wstring selected_path;
  for (auto value : ordinals) {
    if (!selected_path.empty()) selected_path += L'.';
    selected_path += std::to_wstring(value);
  }
  std::wstring executable(32768, L'\0');
  const DWORD length = GetModuleFileNameW(nullptr, executable.data(), executable.size());
  if (!length || length >= executable.size()) return {false, "Cannot locate test executable"};
  executable.resize(length);
  // ASCII encoding preserves UTF-8 names across Windows CRT argv code pages.
  std::wstring encoded;
  constexpr wchar_t digits[] = L"0123456789abcdef";
  for (unsigned char byte : name) {
    encoded += digits[byte >> 4];
    encoded += digits[byte & 15];
  }
  SECURITY_ATTRIBUTES security{sizeof(security), nullptr, TRUE};
  handle read, write;
  if (!CreatePipe(&read.value, &write.value, &security, 0) ||
      !SetHandleInformation(read.value, HANDLE_FLAG_INHERIT, 0))
    return {false, "Cannot create death-test handshake"};
  handle input{inherit_standard(STD_INPUT_HANDLE, GENERIC_READ)};
  handle output{inherit_standard(STD_OUTPUT_HANDLE, GENERIC_WRITE)};
  handle error{inherit_standard(STD_ERROR_HANDLE, GENERIC_WRITE)};
  HANDLE inherited[]{write.value, input.value, output.value, error.value};
  SIZE_T size = 0;
  InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
  std::vector<unsigned char> storage(size);
  auto* attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(storage.data());
  if (!InitializeProcThreadAttributeList(attributes, 1, 0, &size))
    return {false, "Cannot initialize child handle list"};
  struct attribute_cleanup {
    LPPROC_THREAD_ATTRIBUTE_LIST value;
    ~attribute_cleanup() { DeleteProcThreadAttributeList(value); }
  } cleanup{attributes};
  if (!UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherited,
                                 sizeof(inherited), nullptr, nullptr))
    return {false, "Cannot configure child handles"};
  auto command = quote(executable) + L" --annotest-death-name=" + encoded +
                 L" --annotest-death-child=" + selected_path + L":" +
                 std::to_wstring(reinterpret_cast<std::uintptr_t>(write.value));
  STARTUPINFOEXW startup{};
  startup.StartupInfo.cb = sizeof(startup);
  startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
  startup.StartupInfo.hStdInput = input.value;
  startup.StartupInfo.hStdOutput = output.value;
  startup.StartupInfo.hStdError = error.value;
  startup.lpAttributeList = attributes;
  PROCESS_INFORMATION process{};
  if (!CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr, TRUE,
                      EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &startup.StartupInfo,
                      &process))
    return {false, "CreateProcessW() failed: " + std::to_string(GetLastError())};
  handle child{process.hProcess}, thread{process.hThread};
  CloseHandle(write.value);
  write.value = INVALID_HANDLE_VALUE;
  if (WaitForSingleObject(child.value, INFINITE) != WAIT_OBJECT_0)
    return {false, "Cannot wait for death-test child"};
  DWORD code = 0, received = 0, available = 0;
  char marker = 0;
  if (!GetExitCodeProcess(child.value, &code)) return {false, "Cannot read child exit status"};
  if (!PeekNamedPipe(read.value, nullptr, 0, nullptr, &available, nullptr) || !available ||
      !ReadFile(read.value, &marker, 1, &received, nullptr) || received != 1 || marker != 'R')
    return {false, "Child did not reach the death-test callable"};
  if (!code) return {false, "exited normally with status 0"};
  return {true, "exited with status " + std::to_string(code)};
}
#else
std::FILE* temporary_file() { return std::tmpfile(); }
int duplicate(int fd) { return ::dup(fd); }
int redirect(int source, int destination) { return ::dup2(source, destination); }
int close_descriptor(int fd) { return ::close(fd); }
int descriptor(std::FILE* stream) { return ::fileno(stream); }
standard_handles save_standard_handles() { return {}; }
void restore_standard_handles(standard_handles) {}
void sync_standard_handles() {}
std::string capture_stacktrace() {
  void* frames[32];
  int count = ::backtrace(frames, 32);
  char** symbols = ::backtrace_symbols(frames, count);
  std::string result;
  if (symbols) {
    for (int i = 1; i < count; ++i) {
      result += symbols[i];
      result += '\n';
    }
    std::free(symbols);
  }
  return result;
}
std::vector<std::string> utf8_arguments(int, char**) { return {}; }
void configure_death_child(unsigned, std::uintptr_t) {}
bool configure_death_child(std::string_view, std::uintptr_t) { return false; }
bool death_callable_active() { return false; }
bool death_child_active() { return false; }
void reset_death_ordinal() {}
bool enter_death_child() { return false; }
death_result spawn_death_child(std::string_view) { return {false, "Not a Windows backend"}; }
#endif
}  // namespace annotest_platform
