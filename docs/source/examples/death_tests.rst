Check process termination
=========================

Check a fatal path without terminating the parent test runner.
See :ref:`howto-examples-build` for the example build setup.

1. Put the operation inside the callable passed to ``assert_death``.

.. literalinclude:: ../../../examples/example05_death_tests.cpp
   :language: cpp
   :lines: 9-12

2. Forward ``argc`` and ``argv`` to ``run_tests``. Keep setup and the code before the check deterministic: Windows workers replay the case to reach the callable.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example5_death_tests
   ./build/docs-examples/examples/example5_death_tests --test-name DeathTests.nonzero_exit

The child exits with status 2, so the assertion passes and the runner survives. Normal return or exit status zero fails the check. Consult the death-test reference before depending on inherited process state.

See :doc:`/api/testing/death` for API details.
:download:`Runnable source <../../../examples/example05_death_tests.cpp>`.
