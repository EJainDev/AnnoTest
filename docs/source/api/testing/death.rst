.. _death_tests:

Death Tests
===========

``annotest::assertDeath(callable)`` and ``annotest::expectDeath(callable)`` run a
callable in a child process on Linux, macOS, and Windows. A nonzero exit, signal,
or uncaught exception counts as death. Returning normally or exiting with status
zero fails the check. Assertion failure unwinds the body; expectation failure
records a failure and continues.

Linux and macOS
---------------

The child is created with ``fork`` and the parent waits with ``waitpid``, retrying
interrupted waits. The child inherits the parent's memory and file descriptors;
changes to ordinary child memory do not change the parent. Complex shared
resources and background threads still require care.

Windows
-------

Windows relaunches the current executable with an internal selector for the test
and death-check ordinal. It repeats setup and the test body up to that check,
skipping earlier death callables. Named tests, parameterized cases, and multiple
checks in one test are supported. Nested checks are supported on every platform.
For a nested Windows check, the child replays its enclosing death callables to
reach the selected inner callable. A handshake is sent only when that callable
is reached, so startup failures or exceptions in enclosing callables do not
falsely pass the inner check.

Use these helpers inside ``run_tests`` and forward ``argc`` and ``argv`` from
``main``. Setup and code before the death check must be deterministic and safe to
repeat, including enclosing callables for nested checks. State from previously executed tests and parent-only resources are not
inherited. Pointers into the parent process cannot be used in the child.
Replayed setup and body output can appear in captured output.

Example
-------

.. literalinclude:: ../../../../examples/example05_death_tests.cpp
   :language: cpp

Exit status and stderr
----------------------

``assert_exit(callable, exited_with_code(7), "message.*")`` checks the child's
exit code and searches its captured stderr with a regular expression.
``expect_exit`` records a nonfatal failure on mismatch. ``exited_with_code(0)``
accepts a callable returning normally. On POSIX systems,
``killed_by_signal(signal_number)`` checks signal termination. Custom predicates
receive ``exit_status{code, signal}``.

``assertDeath(callable, "message.*")`` and ``expectDeath`` combine nonzero exit
or signal termination with a stderr match. Exceptions continue to count as
process death. Invalid regular expressions raise ``std::regex_error``.
