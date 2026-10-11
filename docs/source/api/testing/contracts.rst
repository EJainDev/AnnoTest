.. _contract_handling:

Contract Handling
=================

For contract checks, include ``<annotest_contract.h>`` in exactly one translation
unit per executable. It imports the library and installs its violation handler.
Enable GCC observe semantics on code containing contracts:

.. code-block:: cmake

    target_compile_options(my_tests PRIVATE
      -fcontracts -fcontract-evaluation-semantic=observe)

``assert_contract_violation`` and ``expect_contract_violation`` check that a
callable violates a contract. ``assert_no_contract_violation`` and
``expect_no_contract_violation`` check that it does not. Callables may return a
value or void; value-returning checks return the callable's result.

Observe semantics continue after a violation, so the body must remain safe with
invalid input. For example, use a guarded fallback rather than divide by zero.
Unexpected violations fail the test. A custom callback can be installed through
``annotest::custom_handler``; the default handler sets
``annotest::contract_violation_occurred`` before invoking it. The runner resets
this flag before each case.

.. literalinclude:: ../../../../examples/example06_contracts.cpp
   :language: cpp
