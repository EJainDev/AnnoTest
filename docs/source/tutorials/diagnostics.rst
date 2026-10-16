Inspect failures without failing the suite
==========================================

.. container:: lesson-intro

   **LESSON 4 OF 5** · Continue from :doc:`parameters`.

   Use a range matcher, inspect an intentionally captured failure, and observe the control-flow difference between assertions and expectations.

1. Describe the whole basket
----------------------------

In ``has_initial_items``, replace the size assertion with an ordered range
matcher. Keep the total check:

.. literalinclude:: ../../../examples/tutorials/04_diagnostics.cpp
   :language: cpp
   :lines: 26-29

This checks both the number of elements and their order. A mismatch describes
the range comparison rather than reporting only a size.

2. Capture a fatal assertion
----------------------------

Add this method inside ``BasketTests``:

.. literalinclude:: ../../../examples/tutorials/04_diagnostics.cpp
   :language: cpp
   :lines: 41-52

The deliberately wrong value creates one failure. Capture redirects it into
``captured``, so it does not fail the outer case. The fatal assertion still
unwinds the lambda: ``reached_end`` remains false. The size assertion guards
access to ``front()``. The trace adds context to the diagnostic.

3. Observe a nonfatal check
---------------------------

Add the companion case:

.. literalinclude:: ../../../examples/tutorials/04_diagnostics.cpp
   :language: cpp
   :lines: 54-63

Here the expectation records a failure and continues to the assignment.
The outer checks verify both the failure category and the continuation.

4. Run and compare
------------------

.. code-block:: sh

   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test

.. admonition:: Check your result
   :class: lesson-result

   **Seven cases pass.** The two deliberate inner failures are captured.
   The fatal check leaves its flag false; the nonfatal check sets its flag true.

Run just the failure-inspection lesson:

.. code-block:: sh

   ./build/first-test/build/first_test \
     --test-name BasketTests.inspect_fatal_assertion BasketTests.expectations_continue

Both selected cases pass. :doc:`/explanation/failures` describes these callable
boundaries; :doc:`/api/testing/assertions` specifies the capture API.

.. container:: lesson-checkpoint

   **Checkpoint** · :download:`Complete main.cpp <../../../examples/tutorials/04_diagnostics.cpp>`.
   Keep the CMake project from lesson 1; only ``main.cpp`` changes.

Continue with :doc:`reports`.
