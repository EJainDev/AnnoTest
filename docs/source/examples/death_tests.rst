Check process termination
=========================

.. container:: guide-intro

   Check a fatal path without terminating the parent test runner.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Move the fatal path into a child
--------------------------------

Put the operation inside the callable passed to ``assert_death``.

.. literalinclude:: ../../../examples/example05_death_tests.cpp
   :language: cpp
   :lines: 9-12

Make replay deterministic
-------------------------

Forward ``argc`` and ``argv`` to ``run_tests``. Keep setup and the code before the check deterministic: Windows workers replay the case to reach the callable.

Run the death check
-------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example5_death_tests
   ./build/docs-examples/examples/example5_death_tests --test-name DeathTests.nonzero_exit

.. admonition:: Expected result
   :class: guide-result

   The child exits with status 2, so the assertion passes and the runner survives. Normal return or exit status zero fails the check. Consult the death-test reference before depending on inherited process state.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/death>` ·
   :download:`Complete runnable source <../../../examples/example05_death_tests.cpp>`
