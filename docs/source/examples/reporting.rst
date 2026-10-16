Save an XML failure report
==========================

Write a report containing case results, failure locations, and captured output.
See :ref:`howto-examples-build` for the example build setup.

1. Pass ``--xml-output`` ``report.xml`` to write a passing report. Inspect its captured stdout and stderr from ``captured_output``.

.. literalinclude:: ../../../examples/example10_reporting.cpp
   :language: cpp
   :lines: 10-15

2. Enable the example's deliberate failure branch with ``ANNOTEST_DEMONSTRATE_FAILURE=1`` and write a second report.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example10_reporting
   ./build/docs-examples/examples/example10_reporting --xml-output report.xml

The normal run passes. The deliberate run below returns 1: two expectations fail and continue, then a fatal assertion unwinds the body. The ``later_test`` case still runs; ``failures.xml`` records three failures and the captured continuation messages.

.. code-block:: sh

   ANNOTEST_DEMONSTRATE_FAILURE=1 ./build/docs-examples/examples/example10_reporting \
     --xml-output failures.xml

See :doc:`/api/testing/command_line` for API details.
:download:`Runnable source <../../../examples/example10_reporting.cpp>`.
