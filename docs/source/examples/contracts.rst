Check contract violations
=========================

Verify a contract's invalid-input and valid-input paths under observe semantics.
See :ref:`howto-examples-build` for the example build setup.

1. Include ``<annotest_contract.h>`` in exactly one translation unit. Compile contract code with ``-fcontracts`` and ``-fcontract-evaluation-semantic=observe``; the example target supplies these flags.

.. literalinclude:: ../../../examples/example06_contracts.cpp
   :language: cpp
   :lines: 9-11

2. Wrap an invalid call in ``assert_contract_violation`` and a valid call in ``assert_no_contract_violation``. Choose an invalid input whose function body remains safe after the violation is observed.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example6_contracts
   ./build/docs-examples/examples/example6_contracts

All four cases pass. Observe mode continues into the function body after a violation. Do not use an invalid input that would then cause division by zero or unchecked memory access.

See :doc:`/api/testing/contracts` for API details.
:download:`Runnable source <../../../examples/example06_contracts.cpp>`.
