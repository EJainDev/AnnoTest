Check contract violations
=========================

.. container:: guide-intro

   Verify a contract's invalid-input and valid-input paths under observe semantics.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Enable the contract handler
---------------------------

Include ``<annotest_contract.h>`` in exactly one translation unit. Compile contract code with ``-fcontracts`` and ``-fcontract-evaluation-semantic=observe``; the example target supplies these flags.

.. literalinclude:: ../../../examples/example06_contracts.cpp
   :language: cpp
   :lines: 9-11

Check both input paths
----------------------

Wrap an invalid call in ``assert_contract_violation`` and a valid call in ``assert_no_contract_violation``. Choose an invalid input whose function body remains safe after the violation is observed.

Run with observe semantics
--------------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example6_contracts
   ./build/docs-examples/examples/example6_contracts

.. admonition:: Expected result
   :class: guide-result

   All four cases pass. Observe mode continues into the function body after a violation. Do not use an invalid input that would then cause division by zero or unchecked memory access.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/contracts>` ·
   :download:`Complete runnable source <../../../examples/example06_contracts.cpp>`
