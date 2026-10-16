Test several input sets
=======================

Cover several input/output pairs without copying the test body.
See :ref:`howto-examples-build` for the example build setup.

1. Make the inputs and expected result arguments of one method. Supply each complete row with ``parameters(...)``.

.. literalinclude:: ../../../examples/example03_parameterized.cpp
   :language: cpp
   :lines: 13-18

2. Add another tuple to cover a new input. Keep its expected result in the tuple so the assertion body stays unchanged.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example3_parameterized
   ./build/docs-examples/examples/example3_parameterized --test-name ParameterizedTests.addition.2

The second row checks 5 + 7 against 12. Select ``ParameterizedTests.addition`` to run all three rows. Use ``--list`` to see the individual numeric suffixes.

See :doc:`/api/testing/annotations` for API details.
:download:`Runnable source <../../../examples/example03_parameterized.cpp>`.
