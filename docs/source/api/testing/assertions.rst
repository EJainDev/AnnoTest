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
