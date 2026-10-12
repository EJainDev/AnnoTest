module;

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

export module annotest:posix;

namespace annotest {
#if defined(__unix__) || defined(__APPLE__)
using pid_t = ::pid_t;
int process_exit_code(int status) { return WIFEXITED(status) ? WEXITSTATUS(status) : -1; }
int process_signal(int status) { return WIFSIGNALED(status) ? WTERMSIG(status) : 0; }

::pid_t fork() { return ::fork(); }

::pid_t waitpid(::pid_t pid, int* status, int options) { return ::waitpid(pid, status, options); }
#endif
}  // namespace annotest