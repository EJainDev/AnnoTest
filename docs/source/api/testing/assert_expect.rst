Assertions and Expectations
===========================

.. cpp:namespace:: cpputils::testing

Assertions
----------

Assertions are used to verify that some feature of the library works as expected. If an assertion fails, an exception is thrown and the test case is marked as failed.

.. cpp:function:: void assertEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are equal using ``operator!=``.

.. cpp:function:: void assertNotEqual(auto expected, auto actual)

    Asserts that ``expected`` and ``actual`` are not equal using ``operator==``.

.. cpp:function:: void assertTrue(auto value)

    Asserts that ``value`` is truthy.

.. cpp:function:: void assertFalse(auto value)

    Asserts that ``value`` is falsy.

.. cpp:function:: void assertNear(auto expected, auto actual, auto tol = 1e-3f)

    Asserts that ``abs(expected - actual)`` is less than or equal to ``tol``.

.. cpp:function:: void assertLess(auto a, auto b)

    Asserts that ``a < b``.

.. cpp:function:: void assertLessEqual(auto a, auto b)

    Asserts that ``a <= b``.

.. cpp:function:: void assertGreater(auto a, auto b)

    Asserts that ``a > b``.

.. cpp:function:: void assertGreaterEqual(auto a, auto b)

    Asserts that ``a >= b``.

.. cpp:function:: void assertContains(const auto& container, const auto& value)

    Asserts that ``value`` exists in ``container`` using ``std::find`` with ``std::begin``/``std::end``.

.. cpp:function:: template <typename E = std::exception> void assertThrows(auto func)

    Asserts that ``func`` throws an exception of type ``E``.

.. cpp:function:: template <typename E> void assertThrowsExact(auto func)

    Asserts that ``func`` throws exactly type ``E`` (checked with ``typeid``).

.. cpp:function:: void assertNull(auto ptr)

    Asserts that ``ptr`` compares equal to ``nullptr``.

Expectations
------------

Expectations are used to verify behavior that should be true for it to be "worth it" for the test to continue. If it is not met, it is not a critical failure or flaw in the library, but it makes running the rest of the test pointless.

.. cpp:function:: void expectEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are equal using ``operator!=``.

.. cpp:function:: void expectNotEqual(auto expected, auto actual)

    Expects that ``expected`` and ``actual`` are not equal using ``operator==``.

.. cpp:function:: void expectTrue(auto value)

    Expects that ``value`` is truthy.

.. cpp:function:: void expectFalse(auto value)

    Expects that ``value`` is falsy.

.. cpp:function:: void expectNear(auto expected, auto actual, auto tol)

    Expects that ``abs(expected - actual)`` is less than or equal to ``tol``.

.. cpp:function:: void expectLess(auto a, auto b)

    Expects that ``a < b``.

.. cpp:function:: void expectLessEqual(auto a, auto b)

    Expects that ``a <= b``.

.. cpp:function:: void expectGreater(auto a, auto b)

    Expects that ``a > b``.

.. cpp:function:: void expectGreaterEqual(auto a, auto b)

    Expects that ``a >= b``.

.. cpp:function:: void expectContains(const auto& container, const auto& value)

    Expects that ``value`` exists in ``container`` using ``std::find`` with ``std::begin``/``std::end``.

.. cpp:function:: template <typename E = std::exception> void expectThrows(auto func)

    Expects that ``func`` throws an exception of type ``E``.

.. cpp:function:: template <typename E> void expectThrowsExact(auto func)

    Expects that ``func`` throws exactly type ``E`` (checked with ``typeid``).

.. cpp:function:: void expectNull(auto ptr)

    Expects that ``ptr`` compares equal to ``nullptr``.