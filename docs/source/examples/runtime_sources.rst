Load cases from runtime data
============================

Turn data loaded at discovery time into individually selectable cases.
See :ref:`howto-examples-build` for the example build setup.

1. Return ``named_case`` values from a provider and connect it to a method with matching arguments using ``parameter_source``.

.. literalinclude:: ../../../examples/example14_runtime_sources.cpp
   :language: cpp
   :lines: 5-12

2. Give rows stable names. With worker processes, discovery and execution must reproduce the same provider results and case names.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example14_runtime_sources
   ./build/docs-examples/examples/example14_runtime_sources --test-name source_tests.twice

The rows check 2 × 2 against 4 and 7 × 2 against 14. Use ``--list`` to inspect their names. For file-backed data, see the CSV and line-reader calls in the linked source.

See :doc:`/api/testing/execution` for API details.
:download:`Runnable source <../../../examples/example14_runtime_sources.cpp>`.
