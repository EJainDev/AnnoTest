Check values and exceptions
===========================

.. container:: guide-intro

   Use an exception check when an operation must reject invalid input.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Wrap the operation
------------------

Pass the operation as a lambda so the check can invoke it.

.. literalinclude:: ../../../examples/example01_basic.cpp
   :language: cpp
   :lines: 17-17

Choose the exception contract
-----------------------------

Set the expected exception type to the error the operation promises. Use a named test when its method name is not descriptive enough.

Run the exception case
----------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example1_basic
   ./build/docs-examples/examples/example1_basic --test-name BasicTests.exception_check

.. admonition:: Expected result
   :class: guide-result

   The selected case passes because accessing index 10 in a three-element vector throws ``std::out_of_range``. Change the callable to the operation under test.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/assertions>` ·
   :download:`Complete runnable source <../../../examples/example01_basic.cpp>`
