.. _contract_handling:

Contract checks
===============

.. cpp:namespace:: annotest

Include ``<annotest_contract.h>`` in exactly one translation unit per
executable to install the contract violation handler. That header imports
AnnoTest. Code containing contracts must use GCC observe semantics:

.. code-block:: cmake

   target_compile_options(my_tests PRIVATE
     -fcontracts -fcontract-evaluation-semantic=observe)

Observe semantics continue after violations. The callable must remain safe
when its contracts are violated. Unexpected violations fail the active case.

.. cpp:function:: template <typename Func> auto assert_contract_violation(Func f, const std::source_location& loc = std::source_location::current())

   Invoke a callable once and require an observed violation. The violation flag is reset before the call and consumed afterward. A mismatch records a fatal assertion and unwinds.

   :param f: Nullary callable returning void or a value.
   :returns: The callable's result by value, or void for a void callable.

   Exceptions from the callable propagate.

.. cpp:function:: template <typename F> decltype(auto) assert_contract_violation(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload with the same check behavior.

   :param callable: Nullary callable.
   :param message: Diagnostic context.
   :param location: Call-site source location.
   :returns: The underlying check's result.

.. cpp:function:: template <typename Func> auto assert_no_contract_violation( Func f, const std::source_location& loc = std::source_location::current())

   Invoke a callable once and require no observed violation. The violation flag is reset before the call and consumed afterward. A mismatch records a fatal assertion and unwinds.

   :param f: Nullary callable returning void or a value.
   :returns: The callable's result by value, or void for a void callable.

   Exceptions from the callable propagate.

.. cpp:function:: template <typename F> decltype(auto) assert_no_contract_violation(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload with the same check behavior.

   :param callable: Nullary callable.
   :param message: Diagnostic context.
   :param location: Call-site source location.
   :returns: The underlying check's result.

.. cpp:function:: template <typename Func> auto expect_contract_violation(Func f, const std::source_location& loc = std::source_location::current())

   Invoke a callable once and require an observed violation. The violation flag is reset before the call and consumed afterward. A mismatch records a nonfatal expectation.

   :param f: Nullary callable returning void or a value.
   :returns: The callable's result by value, or void for a void callable.

   Exceptions from the callable propagate.

.. cpp:function:: template <typename F> decltype(auto) expect_contract_violation(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload with the same check behavior.

   :param callable: Nullary callable.
   :param message: Diagnostic context.
   :param location: Call-site source location.
   :returns: The underlying check's result.

.. cpp:function:: template <typename Func> auto expect_no_contract_violation( Func f, const std::source_location& loc = std::source_location::current())

   Invoke a callable once and require no observed violation. The violation flag is reset before the call and consumed afterward. A mismatch records a nonfatal expectation.

   :param f: Nullary callable returning void or a value.
   :returns: The callable's result by value, or void for a void callable.

   Exceptions from the callable propagate.

.. cpp:function:: template <typename F> decltype(auto) expect_no_contract_violation(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload with the same check behavior.

   :param callable: Nullary callable.
   :param message: Diagnostic context.
   :param location: Call-site source location.
   :returns: The underlying check's result.

Handler state
-------------

.. cpp:var:: inline bool contract_violation_occurred = false

   Flag set by the installed handler before calling custom_handler. The runner resets it before each case. Contract-check helpers reset and consume it around their callable.

.. cpp:var:: inline std::atomic<void (*)(const std::contracts::contract_violation&)> custom_handler

   Optional callback, initially null. Called with the violation after the default handler sets the flag. Assign a compatible function pointer through the atomic.

For a runnable procedure, see :doc:`/examples/contracts`.
