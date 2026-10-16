Reset fixture state
===================

.. container:: guide-intro

   Populate fixture data before each case so test order cannot change its initial state.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Prepare the fixture
-------------------

Store the vector on the fixture and initialize it in ``before_each``. Put explicit cleanup in ``after_each``.

.. literalinclude:: ../../../examples/example02_lifecycle.cpp
   :language: cpp
   :lines: 12-20

Keep each invocation independent
--------------------------------

Keep the default fresh-fixture lifecycle. Every invocation gets a new object and runs the setup hook before its test body.

Run a fresh case
----------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example2_lifecycle
   ./build/docs-examples/examples/example2_lifecycle --test-name LifecycleTests.fresh_fixture

.. admonition:: Expected result
   :class: guide-result

   The case sees three values, starting with 1. Parameterized invocations receive the same setup and cleanup. Use RAII for resources released with the fixture.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/annotations>` ·
   :download:`Complete runnable source <../../../examples/example02_lifecycle.cpp>`
