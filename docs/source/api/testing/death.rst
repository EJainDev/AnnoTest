.. _death_tests:

Process and exit checks
=======================

.. cpp:namespace:: annotest

Process checks run a callable in a child process on Linux, macOS, and Windows.
A fatal check unwinds its callable on mismatch; an expectation records a
nonfatal failure. Call these checks inside a runner that receives main's
unaltered argc and argv.

.. cpp:function:: void assert_death(auto func, const std::source_location& loc = std::source_location::current())

   Require a nonzero exit, signal termination, or uncaught exception. Normal return or exit code zero mismatches.

   :param func: Nullary callable executed in the child.
   :returns: Nothing.

.. cpp:function:: template <typename F> void assert_death(F&& callable, std::string stderr_pattern, std::source_location location = std::source_location::current())

   Require a nonzero exit, signal termination, or uncaught exception. Normal return or exit code zero mismatches.

   :param callable: Nullary callable executed in the child.
   :param stderr_pattern: Regular expression searched in captured child stderr.
   :returns: Nothing.
   :throws std::regex_error: The stderr pattern is invalid.

.. cpp:function:: void expect_death(auto func, const std::source_location& loc = std::source_location::current())

   Require a nonzero exit, signal termination, or uncaught exception. Normal return or exit code zero mismatches.

   :param func: Nullary callable executed in the child.
   :returns: Nothing.

.. cpp:function:: template <typename F> void expect_death(F&& callable, std::string stderr_pattern, std::source_location location = std::source_location::current())

   Require a nonzero exit, signal termination, or uncaught exception. Normal return or exit code zero mismatches.

   :param callable: Nullary callable executed in the child.
   :param stderr_pattern: Regular expression searched in captured child stderr.
   :returns: Nothing.
   :throws std::regex_error: The stderr pattern is invalid.

.. cpp:function:: template <typename F, typename Predicate> void assert_exit(F&& callable, Predicate predicate, std::string stderr_pattern = ".*", std::source_location location = std::source_location::current())

   Require the exit predicate to match and the stderr regex search to succeed. A normal return has exit code zero.

   :param callable: Nullary callable executed in the child.
   :param predicate: Callable accepting exit_status and returning a Boolean-like result.
   :param stderr_pattern: Pattern searched in captured child stderr; defaults to .*, which accepts any content.
   :param location: Call-site source location.
   :returns: Nothing.
   :throws std::regex_error: The pattern is invalid.

.. cpp:function:: template <typename F, typename Predicate> void expect_exit(F&& callable, Predicate predicate, std::string stderr_pattern = ".*", std::source_location location = std::source_location::current())

   Require the exit predicate to match and the stderr regex search to succeed. A normal return has exit code zero.

   :param callable: Nullary callable executed in the child.
   :param predicate: Callable accepting exit_status and returning a Boolean-like result.
   :param stderr_pattern: Pattern searched in captured child stderr; defaults to .*, which accepts any content.
   :param location: Call-site source location.
   :returns: Nothing.
   :throws std::regex_error: The pattern is invalid.

.. cpp:struct:: exit_status

   Exit information passed to a custom exit predicate.

.. cpp:member:: int exit_status::code = -1

   Child exit code when available.

.. cpp:member:: int exit_status::signal = 0

   Terminating POSIX signal; zero when not signal-terminated.

.. cpp:function:: auto exited_with_code(int expected)

   Construct an exit predicate requiring signal == 0 and code == expected.

   :param expected: Required exit code, including zero.
   :returns: Predicate accepting exit_status.

.. cpp:function:: auto killed_by_signal(int expected)

   Construct an exit predicate requiring signal == expected. Signal termination is supported on POSIX platforms.

   :param expected: Required signal number.
   :returns: Predicate accepting exit_status.

Platform requirements
---------------------

On Linux and macOS, a forked child inherits memory and file descriptors.
Changes to ordinary child memory do not update the parent.

Windows relaunches the current executable and replays the selected case up to
the selected check, skipping earlier death callables. Setup and preceding code
must be deterministic and safe to repeat. Parent-only pointers and state
from earlier cases are unavailable. Replay output may appear in captured output.
Failure to reach the selected callable fails the check.

Nested checks are supported. A Windows nested child replays enclosing callables
to reach the selected inner check. For a worked procedure, see
:doc:`/examples/death_tests`.
