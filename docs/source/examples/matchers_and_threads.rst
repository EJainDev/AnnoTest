Check helper-thread results
===========================

.. container:: guide-intro

   Attach checks made by background work to the current test case.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Start a managed test thread
---------------------------

Start the worker with ``test_thread``. Retain its returned thread until the work is complete, and add ``scoped_trace`` for useful failure context.

.. literalinclude:: ../../../examples/example12_matchers_and_threads.cpp
   :language: cpp
   :lines: 23-30

Keep shared data alive
----------------------

Keep test-owned data alive until the worker joins. For a structured result, choose a matcher that describes the required property.

Run the threaded check
----------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example12_matchers_and_threads
   ./build/docs-examples/examples/example12_matchers_and_threads --test-name matcher_tests.helper_threads

.. admonition:: Expected result
   :class: guide-result

   The worker checks 6 × 7 and joins before the case returns. The ``values_and_ranges`` method in the same source shows grouped checks and composed matchers.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/assertions>` ·
   :download:`Complete runnable source <../../../examples/example12_matchers_and_threads.cpp>`
