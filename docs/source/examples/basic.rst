Check values and exceptions
===========================

Use an exception check when an operation must reject invalid input.
See :ref:`howto-examples-build` for the example build setup.

1. Pass the operation as a lambda so the check can invoke it.

.. literalinclude:: ../../../examples/example01_basic.cpp
   :language: cpp
   :lines: 17-17

2. Set the expected exception type to the error the operation promises. Use a named test when its method name is not descriptive enough.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example1_basic
   ./build/docs-examples/examples/example1_basic --test-name BasicTests.exception_check

The selected case passes because accessing index 10 in a three-element vector throws ``std::out_of_range``. Change the callable to the operation under test.

See :doc:`/api/testing/assertions` for API details.
:download:`Runnable source <../../../examples/example01_basic.cpp>`.
