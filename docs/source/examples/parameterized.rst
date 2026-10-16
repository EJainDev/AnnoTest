Test several input sets
=======================

.. container:: guide-intro

   Cover several input/output pairs without copying the test body.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Describe the input rows
-----------------------

Make the inputs and expected result arguments of one method. Supply each complete row with ``parameters(...)``.

.. literalinclude:: ../../../examples/example03_parameterized.cpp
   :language: cpp
   :lines: 13-18

Extend the table
----------------

Add another tuple to cover a new input. Keep its expected result in the tuple so the assertion body stays unchanged.

Run one row or the whole table
------------------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example3_parameterized
   ./build/docs-examples/examples/example3_parameterized --test-name ParameterizedTests.addition.2

.. admonition:: Expected result
   :class: guide-result

   The second row checks 5 + 7 against 12. Select ``ParameterizedTests.addition`` to run all three rows. Use ``--list`` to see the individual numeric suffixes.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/annotations>` ·
   :download:`Complete runnable source <../../../examples/example03_parameterized.cpp>`
