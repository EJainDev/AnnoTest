Test a stateful container
=========================

Check a stack's ordering, error behavior, and round trips in independent cases.
See :ref:`howto-examples-build` for the example build setup.

1. Keep the stack on a fresh fixture. Check the observable sequence of operations, using assertions before later steps depend on a result.

.. literalinclude:: ../../../examples/example07_full_suite.cpp
   :language: cpp
   :lines: 29-37

2. Add a separate exception case for popping an empty stack. Supply several parameter rows for round trips instead of copying that test body.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example7_full_suite
   ./build/docs-examples/examples/example7_full_suite --test-name "Stack.last in first out"

The selected case pushes 1 and 2 and checks that they pop as 2 and 1. The unfinished capacity-limit case remains disabled.

See :doc:`/api/testing/assertions` for API details.
:download:`Runnable source <../../../examples/example07_full_suite.cpp>`.
