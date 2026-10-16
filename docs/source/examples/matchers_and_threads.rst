Check helper-thread results
===========================

Attach checks made by background work to the current test case.
See :ref:`howto-examples-build` for the example build setup.

1. Start the worker with ``test_thread``. Retain its returned thread until the work is complete, and add ``scoped_trace`` for useful failure context.

.. literalinclude:: ../../../examples/example12_matchers_and_threads.cpp
   :language: cpp
   :lines: 23-30

2. Keep test-owned data alive until the worker joins. For a structured result, choose a matcher that describes the required property.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example12_matchers_and_threads
   ./build/docs-examples/examples/example12_matchers_and_threads --test-name matcher_tests.helper_threads

The worker checks 6 × 7 and joins before the case returns. The ``values_and_ranges`` method in the same source shows grouped checks and composed matchers.

See :doc:`/api/testing/assertions` for API details.
:download:`Runnable source <../../../examples/example12_matchers_and_threads.cpp>`.
