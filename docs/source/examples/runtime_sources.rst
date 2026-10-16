Load cases from runtime data
============================

.. container:: guide-intro

   Turn data loaded at discovery time into individually selectable cases.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Connect a provider
------------------

Return ``named_case`` values from a provider and connect it to a method with matching arguments using ``parameter_source``.

.. literalinclude:: ../../../examples/example14_runtime_sources.cpp
   :language: cpp
   :lines: 5-12

Keep case identities stable
---------------------------

Give rows stable names. With worker processes, discovery and execution must reproduce the same provider results and case names.

Run the loaded cases
--------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example14_runtime_sources
   ./build/docs-examples/examples/example14_runtime_sources --test-name source_tests.twice

.. admonition:: Expected result
   :class: guide-result

   The rows check 2 × 2 against 4 and 7 × 2 against 14. Use ``--list`` to inspect their names. For file-backed data, see the CSV and line-reader calls in the linked source.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/execution>` ·
   :download:`Complete runnable source <../../../examples/example14_runtime_sources.cpp>`
