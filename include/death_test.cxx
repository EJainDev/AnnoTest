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
struct death_result {
  bool died;
  std::string detail;
  int exit_code = -1, signal = 0;
  bool reached = false;
};

#if defined(__unix__) || defined(__APPLE__)
template <typename F>
death_result run_death_test(F&& func) {
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
    return {false, "exited normally with status 0", 0, 0, true};
  }
  return {true, std::format("exited with status {}", status), process_exit_code(status),
          process_signal(status), true};
}
#elif defined(_WIN32)
template <typename F>
death_result run_death_test(F&& func) {
  if (annotest_platform::death_child_active() && !annotest_platform::death_callable_active()) {
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
  return {result.died, std::move(result.detail), result.exit_code, result.signal, result.reached};
}
#else
#error "AnnoTest requires Windows or a POSIX platform"
#endif

export void assert_death(auto func,
                         const std::source_location& loc = std::source_location::current()) {
  auto result = run_death_test(std::move(func));
  if (!result.died) {
    add_failure("Assertion failed: expected process death, child " + result.detail, loc);
  }
}

export void expect_death(auto func,
                         const std::source_location& loc = std::source_location::current()) {
  auto result = run_death_test(std::move(func));
  if (!result.died) {
    add_failure_expect("Expectation failed: expected process death, child " + result.detail, loc);
  }
}

export struct exit_status {
  int code = -1, signal = 0;
};
export inline auto exited_with_code(int expected) {
  return [expected](exit_status result) { return result.signal == 0 && result.code == expected; };
}
export inline auto killed_by_signal(int expected) {
  return [expected](exit_status result) { return result.signal == expected; };
}
template <typename F, typename Predicate>
bool check_exit(F&& callable, Predicate predicate, const std::string& pattern,
                std::string& detail) {
#ifdef _WIN32
  if (annotest_platform::death_child_active() && !annotest_platform::death_callable_active()) {
    return run_death_test(std::forward<F>(callable)).died;
  }
#endif
  std::regex expression(pattern);
  auto* capture = annotest_platform::temporary_file();
  if (!capture) {
    detail = "Cannot capture child stderr";
    return false;
  }
  struct file_guard {
    std::FILE* file;
    ~file_guard() { std::fclose(file); }
  } close{capture};
  std::fflush(stderr);
  int saved = annotest_platform::duplicate(annotest_platform::descriptor(stderr));
  if (saved < 0) {
    detail = "Cannot save stderr";
    return false;
  }
  struct redirect_guard {
    int saved;
    ~redirect_guard() {
      std::fflush(stderr);
      annotest_platform::redirect(saved, annotest_platform::descriptor(stderr));
      annotest_platform::close_descriptor(saved);
      annotest_platform::sync_standard_handles();
    }
  } restore{saved};
  if (annotest_platform::redirect(annotest_platform::descriptor(capture),
                                  annotest_platform::descriptor(stderr)) < 0) {
    detail = "Cannot redirect child stderr";
    return false;
  }
  annotest_platform::sync_standard_handles();
  auto result = run_death_test(std::forward<F>(callable));
  std::fflush(stderr);
  std::rewind(capture);
  std::string text;
  for (int c; (c = std::fgetc(capture)) != EOF;) text += static_cast<char>(c);
  detail = result.detail + "; stderr: " + text;
  return result.reached && std::invoke(predicate, exit_status{result.exit_code, result.signal}) &&
         std::regex_search(text, expression);
}
export template <typename F, typename Predicate>
void assert_exit(F&& callable, Predicate predicate, std::string stderr_pattern = ".*",
                 std::source_location location = std::source_location::current()) {
  std::string detail;
  if (!check_exit(std::forward<F>(callable), predicate, stderr_pattern, detail))
    add_failure("Unexpected child termination: " + detail, location);
}
export template <typename F, typename Predicate>
void expect_exit(F&& callable, Predicate predicate, std::string stderr_pattern = ".*",
                 std::source_location location = std::source_location::current()) {
  std::string detail;
  if (!check_exit(std::forward<F>(callable), predicate, stderr_pattern, detail))
    add_failure_expect("Unexpected child termination: " + detail, location);
}
export template <typename F>
void assert_death(F&& callable, std::string stderr_pattern,
                  std::source_location location = std::source_location::current()) {
  assert_exit(
      std::forward<F>(callable),
      [](exit_status status) { return status.signal != 0 || status.code != 0; },
      std::move(stderr_pattern), location);
}
export template <typename F>
void expect_death(F&& callable, std::string stderr_pattern,
                  std::source_location location = std::source_location::current()) {
  expect_exit(
      std::forward<F>(callable),
      [](exit_status status) { return status.signal != 0 || status.code != 0; },
      std::move(stderr_pattern), location);
}
}  // namespace annotest