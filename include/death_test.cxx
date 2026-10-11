module;
#include <cerrno>

#include "annotest_platform.h"

export module annotest:death_test;

import :asserts;
import :expects;
import :posix;
import :logger;

import std;

namespace annotest {
struct DeathResult {
  bool died;
  std::string detail;
};

#if defined(__unix__) || defined(__APPLE__)
template <typename F>
DeathResult runDeathTest(F&& func) {
  const pid_t pid = fork();

  if (pid < 0) {
    return {false, "fork() failed"};
  }

  // Note: The child inherits all file descriptors, signal handlers, and
  // atexit handlers from the parent. Avoid complex parent state when using
  // death tests — the child's lifetime is bounded by func()'s execution.
  // It is the child if pid == 0
  if (pid == 0) {
    try {
      func();
      std::_Exit(0);
    } catch (...) {
      std::_Exit(1);
    }
  }

  int status = 0;
  pid_t waited;
  do {
    waited = waitpid(pid, &status, 0);
  } while (waited < 0 && errno == EINTR);
  if (waited < 0) {
    return {false, "waitpid() failed"};
  }

  if (status == 0) {
    return {false, "exited normally with status 0"};
  }
  return {true, std::format("exited with status {}", status)};
}
#elif defined(_WIN32)
template <typename F>
DeathResult runDeathTest(F&& func) {
  if (annotest_platform::death_child_active()) {
    if (!annotest_platform::enter_death_child()) return {true, "earlier death check replayed"};
    try {
      func();
      std::_Exit(0);
    } catch (...) {
      std::_Exit(1);
    }
  }
  if (suite_log.testcases.empty()) return {false, "Death tests require an active test runner"};
  auto result =
      annotest_platform::spawn_death_child(suite_log.name + "." + suite_log.testcases.back().name);
  return {result.died, std::move(result.detail)};
}
#else
#error "AnnoTest requires Windows or a POSIX platform"
#endif

export void assertDeath(auto func,
                        const std::source_location& loc = std::source_location::current()) {
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    add_failure("Assertion failed: expected process death, child " + result.detail, loc);
  }
}

export void expectDeath(auto func,
                        const std::source_location& loc = std::source_location::current()) {
  auto result = runDeathTest(std::move(func));
  if (!result.died) {
    add_failure_expect("Expectation failed: expected process death, child " + result.detail, loc);
  }
}

}  // namespace annotest