Test several types
==================

Apply one behavioral check to several concrete type specializations.
See :ref:`howto-examples-build` for the example build setup.

1. Make the test method a template. Give it default template arguments so reflection can inspect it, and list the concrete types in ``parameterize_template``.

.. literalinclude:: ../../../examples/example08_typed.cpp
   :language: cpp
   :lines: 12-22

2. Add a ``template_params`` entry for each extra type that should satisfy the same behavior. Use ``--list`` to inspect the generated cases.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example8_typed
   ./build/docs-examples/examples/example8_typed --test-name TypedTests.vector_round_trip

The vector round trip runs for ``int`` and ``double``. Selecting the base method name runs both concrete invocations.

See :doc:`/api/testing/annotations` for API details.
:download:`Runnable source <../../../examples/example08_typed.cpp>`.
