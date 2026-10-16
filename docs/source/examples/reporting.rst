Save an XML failure report
==========================

.. container:: guide-intro

   Write a report containing case results, failure locations, and captured output.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Capture output in a report
--------------------------

Pass ``--xml-output`` ``report.xml`` to write a passing report. Inspect its captured stdout and stderr from ``captured_output``.

.. literalinclude:: ../../../examples/example10_reporting.cpp
   :language: cpp
   :lines: 10-15

Enable the failure demonstration
--------------------------------

Enable the example's deliberate failure branch with ``ANNOTEST_DEMONSTRATE_FAILURE=1`` and write a second report.

Write and inspect the reports
-----------------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example10_reporting
   ./build/docs-examples/examples/example10_reporting --xml-output report.xml

.. admonition:: Expected result
   :class: guide-result

   The normal run passes. The deliberate run below returns 1: two expectations fail and continue, then a fatal assertion unwinds the body. The ``later_test`` case still runs; ``failures.xml`` records three failures and the captured continuation messages.

.. code-block:: sh

   ANNOTEST_DEMONSTRATE_FAILURE=1 ./build/docs-examples/examples/example10_reporting \
     --xml-output failures.xml

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/command_line>` ·
   :download:`Complete runnable source <../../../examples/example10_reporting.cpp>`
