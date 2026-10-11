.. _assertions_and_expectations:

Assertions and Expectations
===========================

.. cpp:namespace:: annotest

Assertions and expectations verify conditions during tests. They differ in how they handle failures:

.. list-table::
   :widths: 20 25 30 25
   :stub-columns: 1
   :header-rows: 1

   * - Category
     - Prefix
     - On Failure
     - Continues?
   * - **Assert**
     - ``assert*``
     - Throws :cpp:class:`assertion_error`
     - Marks test **failed**, continues suite
   * - **Expect**
     - ``expect*``
     - Throws :cpp:class:`Abort`
     - **Aborts** current test, continues suite

**Use assertions** for conditions that, if false, mean the test result is invalid.
**Use expectations** for conditions that, if false, mean subsequent test code is meaningless, but the test result is still valid (e.g., setup failures).

Assertions
----------

Assertions verify that a condition holds. If an assertion fails, an :cpp:class:`assertion_error` exception is thrown, the test case is marked as failed, and the library continues with the next test.

.. cpp:function:: void assert_equal(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are equal (checked via ``!=``).

    .. code-block:: cpp

        assert_equal(4, 2 + 2);
        assert_equal(std::vector{1, 2}, std::vector{1, 2});

.. cpp:function:: void assert_not_equal(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are not equal (checked via ``==``).

    .. code-block:: cpp

        assert_not_equal(3, 4);

.. cpp:function:: void assert_true(auto value)

    Asserts that ``value`` is truthy.

    .. code-block:: cpp

        assert_true(true);
        assert_true(1 == 1);
        assert_true(5 > 3);

.. cpp:function:: void assert_false(auto value)

    Asserts that ``value`` is falsy.

    .. code-block:: cpp

        assert_false(false);
        assert_false(1 == 0);

.. cpp:function:: void assert_near(auto expected, auto actual, auto tol = 0.001)

    Asserts that ``abs(expected - actual) <= tol``. Useful for floating-point comparisons.

    .. code-block:: cpp

        assert_near(3.14159, 3.14158, 1e-3);
        assert_near(5.0, 5.00001, 1e-4);

.. cpp:function:: void assert_less(auto a, auto b)

    Asserts that ``a < b``.

    .. code-block:: cpp

        assert_less(2, 5);

.. cpp:function:: void assert_less_equal(auto a, auto b)

    Asserts that ``a <= b``.

    .. code-block:: cpp

        assert_less_equal(5, 5);
        assert_less_equal(4, 5);

.. cpp:function:: void assert_greater(auto a, auto b)

    Asserts that ``a > b``.

    .. code-block:: cpp

        assert_greater(10, 1);

.. cpp:function:: void assert_greater_equal(auto a, auto b)

    Asserts that ``a >= b``.

    .. code-block:: cpp

        assert_greater_equal(5, 5);
        assert_greater_equal(10, 5);

.. cpp:function:: void assert_contains(auto const& container, auto const& value)

    Asserts that ``value`` exists in ``container`` (uses ``std::find``).

    .. code-block:: cpp

        std::vector<int> nums{1, 2, 3, 4, 5};
        assert_contains(nums, 3);

.. cpp:function:: template <typename E = std::exception> void assert_throws(auto func)

    Asserts that ``func`` throws an exception. By default, catches any exception derived from ``std::exception`` (which covers all standard exceptions). For meaningful type-specific checking, always provide an explicit template argument.

    .. code-block:: cpp

        // Catches any std::exception-derived exception
        assert_throws([]() { throw std::runtime_error("assertion_error"); });
        assert_throws([]() { throw std::invalid_argument("bad"); });

        // For type-specific checking, use an explicit type
        assert_throws<std::runtime_error>([]() { throw std::runtime_error("exact match"); });

.. cpp:function:: template <typename E> void assert_throws_exact(auto func)

    Asserts that ``func`` throws **exactly** type ``E`` (checked with ``typeid``). Unlike ``assert_throws``, this rejects derived types — the thrown exception must be precisely ``E``.

    .. code-block:: cpp

        assert_throws_exact<std::runtime_error>([]() { throw std::runtime_error("exact"); });
        // The following would FAIL (different type, even if derived):
        // assert_throws_exact<std::exception>([]() { throw std::runtime_error("assertion_error"); });

.. cpp:function:: void assert_null(auto ptr)

    Asserts that ``ptr`` equals ``nullptr``.

    .. code-block:: cpp

        assert_null(nullptr);

.. cpp:function:: void assertDeath(auto func)

    Asserts that ``func`` causes the child process to exit abnormally (via ``fork`` / ``waitpid``). Unix and macOS only.

    .. code-block:: cpp

        assertDeath([]() { throw std::runtime_error("crash!"); });
        assertDeath([]() { std::abort(); });

.. cpp:function:: void assert_contract_violation(Func f)

    Asserts that calling ``f`` triggers a C++26 contract violation.

    .. code-block:: cpp

        int safe_divide(int a, int b) pre(b > 0) { return a / b; }

        assert_contract_violation([]() { safe_divide(10, 0); });  // Triggers pre(b > 0)

.. cpp:function:: void assert_no_contract_violation(Func f)

    Asserts that calling ``f`` does **not** trigger a contract violation.

    .. code-block:: cpp

        assert_no_contract_violation([]() { safe_divide(10, 2); });  // OK

Expectations
------------

Expectations verify conditions that are **not** critical failures — if an expectation fails, the test is aborted and the library skips the remaining lines in that test case. An :cpp:class:`Abort` exception is thrown instead of :cpp:class:`assertion_error`.

.. cpp:function:: void expectEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are equal.

    .. code-block:: cpp

        expectEqual(4, 2 + 2);

.. cpp:function:: void expectNotEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are not equal.

    .. code-block:: cpp

        expectNotEqual(3, 4);

.. cpp:function:: void expectTrue(auto value)

    Expects that ``value`` is truthy.

.. cpp:function:: void expectFalse(auto value)

    Expects that ``value`` is falsy.

.. cpp:function:: void expectNear(auto expected, auto actual, auto tol = 0.001)

    Expects that ``abs(expected - actual) <= tol``. Has the same default tolerance as ``assert_near``.

.. cpp:function:: void expectLess(auto a, auto b)

    Expects that ``a < b``.

.. cpp:function:: void expectLessEqual(auto a, auto b)

    Expects that ``a <= b``.

.. cpp:function:: void expectGreater(auto a, auto b)

    Expects that ``a > b``.

.. cpp:function:: void expectGreaterEqual(auto a, auto b)

    Expects that ``a >= b``.

.. cpp:function:: void expectContains(auto const& container, auto const& value)

    Expects that ``value`` exists in ``container``.

.. cpp:function:: template <typename E = std::exception> void expectThrows(auto func)

    Expects that ``func`` throws an exception. By default, catches any exception derived from ``std::exception`` (which covers all standard exceptions). For meaningful type-specific checking, always provide an explicit template argument.

.. cpp:function:: template <typename E> void expectThrowsExact(auto func)

    Expects that ``func`` throws **exactly** type ``E`` (checked with ``typeid``). Unlike ``expectThrows``, this rejects derived types — the thrown exception must be precisely ``E``.

.. cpp:function:: void expectNull(auto ptr)

    Expects that ``ptr`` equals ``nullptr``.

    .. code-block:: cpp

        expectNull(nullptr);

.. cpp:function:: void expectDeath(auto func)

    Expects that ``func`` causes the child process to exit abnormally. Unix and macOS only.

    .. code-block:: cpp

        expectDeath([]() { throw std::runtime_error("die"); });

.. cpp:function:: void expectContractViolation(Func f)

    Expects that calling ``f`` triggers a C++26 contract violation.

.. cpp:function:: void expectNoContractViolation(Func f)

    Expects that calling ``f`` does **not** trigger a contract violation.
