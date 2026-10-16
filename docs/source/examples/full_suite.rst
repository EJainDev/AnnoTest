Test a stateful container
=========================

.. container:: guide-intro

   Check a stack's ordering, error behavior, and round trips in independent cases.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Keep state on the fixture
-------------------------

Keep the stack on a fresh fixture. Check the observable sequence of operations, using assertions before later steps depend on a result.

.. literalinclude:: ../../../examples/example07_full_suite.cpp
   :language: cpp
   :lines: 29-37

Separate independent behavior
-----------------------------

Add a separate exception case for popping an empty stack. Supply several parameter rows for round trips instead of copying that test body.

Run the ordering case
---------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example7_full_suite
   ./build/docs-examples/examples/example7_full_suite --test-name "Stack.last in first out"

.. admonition:: Expected result
   :class: guide-result

   The selected case pushes 1 and 2 and checks that they pop as 2 and 1. The unfinished capacity-limit case remains disabled.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/assertions>` ·
   :download:`Complete runnable source <../../../examples/example07_full_suite.cpp>`
