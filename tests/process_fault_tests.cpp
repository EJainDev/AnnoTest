#include <spawn.h>
#include <sys/wait.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "annotest_platform.h"

static std::string mode;
static bool injected = false;
static int opens = 0;
static int stage = 0;
extern "C" int __real_posix_spawn_file_actions_init(posix_spawn_file_actions_t*);
extern "C" int __wrap_posix_spawn_file_actions_init(posix_spawn_file_actions_t* actions) {
  if (mode == "files-init") return ENOMEM;
  return __real_posix_spawn_file_actions_init(actions);
}
extern "C" int __real_posix_spawn_file_actions_addopen(posix_spawn_file_actions_t*, int,
                                                       const char*, int, mode_t);
extern "C" int __wrap_posix_spawn_file_actions_addopen(posix_spawn_file_actions_t* actions, int fd,
                                                       const char* path, int flags,
                                                       mode_t permissions) {
  ++opens;
  if (mode == "open-first" || (mode == "open-second" && opens == 2)) return ENOMEM;
  return __real_posix_spawn_file_actions_addopen(actions, fd, path, flags, permissions);
}
extern "C" int __real_posix_spawnattr_init(posix_spawnattr_t*);
extern "C" int __wrap_posix_spawnattr_init(posix_spawnattr_t* attributes) {
  if (mode == "attributes") return ENOMEM;
  return __real_posix_spawnattr_init(attributes);
}
extern "C" pid_t __real_waitpid(pid_t, int*, int);
extern "C" pid_t __wrap_waitpid(pid_t pid, int* status, int flags) {
  if (mode == "wait-error-interrupt" || mode == "wait-error-terminal") {
    if (flags == WNOHANG && stage == 0) {
      stage = 1;
      errno = ECHILD;
      return -1;
    }
    if (flags == 0 && stage == 1) {
      stage = 2;
      if (mode == "wait-error-terminal") __real_waitpid(pid, status, flags);
      errno = mode == "wait-error-terminal" ? ECHILD : EINTR;
      return -1;
    }
  }
  if (mode == "timeout-terminal" && flags == 0 && !injected) {
    injected = true;
    __real_waitpid(pid, status, flags);
    errno = ECHILD;
    return -1;
  }
  if (!injected && (mode == "wait-error" || mode == "wait-interrupt" ||
                    (mode == "timeout-interrupt" && flags == 0))) {
    injected = true;
    errno = mode == "wait-error" ? ECHILD : EINTR;
    return -1;
  }
  return __real_waitpid(pid, status, flags);
}
int main(int argc, char** argv) {
  if (argc >= 2 && std::string(argv[1]) == "--child") {
    if (argc >= 3 && std::string(argv[2]) == "hang")
      std::this_thread::sleep_for(std::chrono::seconds(10));
    std::puts("child output");
    std::fputs("child error\n", stderr);
    return 0;
  }
  if (argc != 3) return 2;
  mode = argv[1];
  auto directory = std::filesystem::path(argv[2]);
  auto output = (directory / "stdout").string(), error = (directory / "stderr").string();
  std::vector<std::string> args{argv[0], "--child"};
  if (mode == "empty") args.clear();
  if (mode == "missing") args = {"/annotest-does-not-exist"};
  if (mode == "hang" || mode == "timeout-interrupt" || mode == "timeout-terminal")
    args.push_back("hang");
  double timeout =
      (mode == "hang" || mode == "timeout-interrupt" || mode == "timeout-terminal") ? 0.05 : 1.0;
  auto result = annotest_platform::run_process(args, output, error, timeout);
  if (mode == "wait-interrupt") {
    if (result.exit_code != 0 || result.timed_out || !result.detail.empty()) return 3;
  } else if (mode == "hang" || mode == "timeout-interrupt" || mode == "timeout-terminal") {
    if (!result.timed_out || result.exit_code != 1) return 4;
  } else if (result.exit_code != -1 || result.detail.empty())
    return 5;
  std::puts(result.detail.c_str());
  return 0;
}
