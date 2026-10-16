Reset fixture state
===================

Populate fixture data before each case so test order cannot change its initial state.
See :ref:`howto-examples-build` for the example build setup.

1. Store the vector on the fixture and initialize it in ``before_each``. Put explicit cleanup in ``after_each``.

.. literalinclude:: ../../../examples/example02_lifecycle.cpp
   :language: cpp
   :lines: 12-20

2. Keep the default fresh-fixture lifecycle. Every invocation gets a new object and runs the setup hook before its test body.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example2_lifecycle
   ./build/docs-examples/examples/example2_lifecycle --test-name LifecycleTests.fresh_fixture

The case sees three values, starting with 1. Parameterized invocations receive the same setup and cleanup. Use RAII for resources released with the fixture.

See :doc:`/api/testing/annotations` for API details.
:download:`Runnable source <../../../examples/example02_lifecycle.cpp>`.
