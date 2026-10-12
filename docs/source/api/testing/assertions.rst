.. _assertions_and_expectations:

Assertions and Expectations
===========================

A failed ``assert_*`` records a failure and unwinds the current body; a failed
``expect_*`` records a failure and continues. Both fail the case and allow later
cases to run. The internal assertion exception is not a public API. Avoid
catching assertion failures yourself.

.. list-table:: Checks in the annotest namespace
   :header-rows: 1

   * - Purpose
     - Assertion
     - Expectation
   * - Equality
     - ``assert_equal(expected, actual)``
     - ``expect_equal(expected, actual)``
   * - Inequality
     - ``assert_not_equal(expected, actual)``
     - ``expect_not_equal(expected, actual)``
   * - Truth
     - ``assert_true(value)``, ``assert_false(value)``
     - ``expect_true(value)``, ``expect_false(value)``
   * - Numeric tolerance
     - ``assert_near(expected, actual, tolerance)``
     - ``expect_near(expected, actual, tolerance)``
   * - Ordering
     - ``assert_less``, ``assert_less_equal``, ``assert_greater``, ``assert_greater_equal``
     - Corresponding ``expect_*`` helpers
   * - Containment
     - ``assert_contains(container, value)``
     - ``expect_contains(container, value)``
   * - Null pointer
     - ``assert_null(pointer)``
     - ``expect_null(pointer)``
   * - Exception matching
     - ``assert_throws<E>(callable)``, ``assert_throws_exact<E>(callable)``
     - ``expect_throws<E>(callable)``, ``expect_throws_exact<E>(callable)``
   * - Contract violation
     - ``assert_contract_violation``, ``assert_no_contract_violation``
     - Corresponding ``expect_*`` helpers
   * - Process death
     - ``assertDeath(callable)``
     - ``expectDeath(callable)``

The death helpers retain camel-case names and support all three platforms; see
:doc:`death` for the Windows replay requirements. The tolerance default is 0.001.
Exception helpers default to ``std::exception`` and match derived types;
``*_throws_exact`` requires the precise exception type. All checks accept an
optional source location, defaulted to the call site.

Checks evaluate their operands/callables once. Diagnostic formatting is deferred
until a check fails. ``wrap`` allows deferred or referenced values; see the basic
example. Contract checks require the handler header and observe semantics
explained in :doc:`contracts`.

.. code-block:: cpp

    [[= test{}]] void checks() {
        expect_equal(4, 2 + 2);
        assert_throws<std::runtime_error>([] { throw std::runtime_error("example"); });
        expect_true(true);  // Runs after successful checks and failed expectations.
    }

Messages and failure inspection
-------------------------------

Pass a message after the check arguments and before the optional source location:
``expect_equal(expected, actual, "row 7")``. The message appears in the failure
diagnostic. This form also applies to truth, ordering, containment, tolerance,
null, exception, and contract checks, and to ``assert_no_throw`` and
``expect_no_throw``.

``has_failure()``, ``has_fatal_failure()``, and ``has_nonfatal_failure()`` inspect
the active case. ``current_failures()`` returns its recorded failures, including
type, message, phase, and source location.

``capture_failures(callable)`` records a callable's assertion failures in a
returned vector without failing the surrounding test. Fatal assertions unwind
the callable. Nested captures are independent; other exceptions propagate.
Pass ``true`` as the second argument to capture failures from helper threads too.
Join those threads before the callable returns.

``assert_failure(callable, failure_types::assertion, "message fragment")`` and
``expect_failure`` check that a callable produces a matching failure.
``assert_no_fatal_failure`` and ``expect_no_fatal_failure`` run a callable and
report a fatal assertion from it as a failed check.

Helper threads
--------------

Assertions from helper threads attach to the active case in arrival order.
Join helper threads before the test returns. Scoped traces belong to the thread
that creates them, and listener assertion callbacks are serialized.

``test_thread(callable)`` returns a ``std::jthread`` that joins on destruction.
Fatal assertions unwind its callable; unexpected exceptions are recorded as
failures on the active case. Nonfatal expectations continue within the thread.

String comparisons and diagnostic printers
-----------------------------------------

``assert_streq`` / ``expect_streq`` compare C strings by content, and
``assert_strne`` / ``expect_strne`` check that their contents differ.
``assert_strcaseeq`` / ``expect_strcaseeq`` and
``assert_strcasene`` / ``expect_strcasene`` ignore ASCII letter case.
These helpers accept narrow or wide C strings, followed by an optional message
and source location. Two null pointers are equal; null and an empty string are
different. Comparisons stop at the first null terminator. Unicode case folding
is not performed. ``assert_equal`` and ``expect_equal`` continue to use ordinary
operator equality, including pointer identity for C strings.

.. code-block:: cpp

    char first[] = "hello", second[] = "hello";
    assert_streq(first, second);       // Content equality.
    assert_not_equal(first, second);   // Distinct addresses.
    expect_strcaseeq("HELLO", "hello", "response header");

String assertion failures show quoted, escaped values and the first differing
character index for equality checks. Wide strings are encoded as UTF-8;
invalid wide code units remain visible in escaped form. Null C strings are
printed as ``nullptr``. Character arrays are bounded when printed directly.
Ordering failures include the comparison operator and both operands.
Generic C-string equality failures also show pointer addresses to distinguish
different buffers containing identical text.

Define ``void PrintTo(const T&, std::ostream*)`` in the same namespace as your
type, or as a friend, to supply a diagnostic printer. Argument-dependent lookup
finds it before the standard formatter, range printer, or stream fallback.
The same printer is used inside ranges and first-difference diagnostics.
Assertion diagnostics invoke printers only on failure.

.. code-block:: cpp

    struct Point {
        int x, y;
        friend void PrintTo(const Point& point, std::ostream* out) {
            *out << "Point(" << point.x << ", " << point.y << ")";
        }
    };

Function-based assertions cannot automatically recover operand expression text
from the call site. Use the optional message or ``scoped_trace`` to name the
expressions or input context you want included in diagnostics.

Grouped assertions
------------------

``assert_all("label", block1, block2, ...)`` executes independent callable blocks
in order. A fatal assertion ends its block, and later blocks still run. After
all blocks finish, the group unwinds if any new failure was recorded.
``expect_all`` runs the same blocks and continues after the group even when
they fail. Labels are optional; groups may be empty or nested and accept
move-only callables.

.. code-block:: cpp

    assert_all("response",
        [&] {
            assert_not_equal(response, nullptr);
            assert_equal(200, response->status);
        },
        [&] { assert_streq(version, "1.0"); });

Original failure types and assertion locations are retained, with the group
label added as trace context. Standard and unknown exceptions are recorded as
errors and do not prevent later blocks from running. A skip immediately aborts
the group and remaining blocks. Groups do not duplicate recorded failures and
work inside ``capture_failures``; intentionally intercepted inner failures do
not fail the surrounding group.
