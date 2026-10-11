#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>

import annotest;
import std;

using namespace annotest;

namespace {
std::string_view scenario;
int tmp_calls = 0, dup_calls = 0, redirect_calls = 0;
bool injected = false;
bool fail(std::string_view name) {
  if (scenario == name && !injected) {
    injected = true;
    return true;
  }
  return false;
}
std::size_t open_descriptors() {
  return std::distance(std::filesystem::directory_iterator("/proc/self/fd"),
                       std::filesystem::directory_iterator{});
}
}  // namespace

extern "C" {
FILE* __real_tmpfile();
int __real_dup(int);
int __real_dup2(int, int);
pid_t __real_fork();
pid_t __real_waitpid(pid_t, int*, int);

FILE* __wrap_tmpfile() {
  ++tmp_calls;
  if (fail(tmp_calls == 1 ? "tmp_stdout" : "tmp_stderr")) return nullptr;
  return __real_tmpfile();
}
int __wrap_dup(int descriptor) {
  ++dup_calls;
  if (fail(dup_calls == 1 ? "dup_stdout" : "dup_stderr")) return -1;
  return __real_dup(descriptor);
}
int __wrap_dup2(int source, int destination) {
  ++redirect_calls;
  if (fail(redirect_calls == 1 ? "redirect_stdout" : "redirect_stderr")) return -1;
  return __real_dup2(source, destination);
}
pid_t __wrap_fork() {
  if (fail("fork")) return -1;
  return __real_fork();
}
pid_t __wrap_waitpid(pid_t pid, int* status, int options) {
  if (fail("wait_interrupted")) {
    errno = EINTR;
    return -1;
  }
  auto result = __real_waitpid(pid, status, options);
  // Reap the real child before simulating the error, so the fixture leaves no zombie.
  if (fail("wait")) {
    errno = ECHILD;
    return -1;
  }
  return result;
}
}

#ifdef ANNOTEST_COVERAGE
// Flush only in the instrumented fixture; ordinary builds retain normal _Exit semantics.
extern "C" void __gcov_dump();
extern "C" [[noreturn]] void __real__Exit(int);
extern "C" [[noreturn]] void __wrap__Exit(int status) {
  __gcov_dump();
  __real__Exit(status);
}
#endif

struct FaultTests {
  [[= test{}]] void output() {
    std::println("body executed");
    std::println(std::cerr, "stderr executed");
  }
  [[= test{}]] void assert_death() {
    assertDeath([] { std::_Exit(1); });
  }
  [[= test{}]] void expect_death() {
    expectDeath([] { std::_Exit(1); });
    std::println("continued");
  }
  [[= test{}]] void child_return() {
    assertDeath([] {});
  }
  [[= test{}]] void child_exception() {
    expectDeath([] { throw 42; });
  }
};

int main(int argc, char** argv) {
  auto mode = std::getenv("ANNOTEST_SCENARIO");
  scenario = mode ? mode : "";
  const auto descriptors = open_descriptors();
  try {
    return run_tests<FaultTests>(argc, argv);
  } catch (const std::runtime_error& error) {
    if (!injected || scenario == "fork" || scenario == "wait") return 2;
    if (open_descriptors() != descriptors) return 3;
    std::println("caught: {}", error.what());
    std::println("stdout restored");
    std::println(std::cerr, "stderr restored");
    return 0;
  }
}
