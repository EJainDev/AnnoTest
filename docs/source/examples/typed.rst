Test several types
==================

.. container:: guide-intro

   Apply one behavioral check to several concrete type specializations.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Declare the type specializations
--------------------------------

Make the test method a template. Give it default template arguments so reflection can inspect it, and list the concrete types in ``parameterize_template``.

.. literalinclude:: ../../../examples/example08_typed.cpp
   :language: cpp
   :lines: 12-22

Extend the type list
--------------------

Add a ``template_params`` entry for each extra type that should satisfy the same behavior. Use ``--list`` to inspect the generated cases.

Run the typed cases
-------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example8_typed
   ./build/docs-examples/examples/example8_typed --test-name TypedTests.vector_round_trip

.. admonition:: Expected result
   :class: guide-result

   The vector round trip runs for ``int`` and ``double``. Selecting the base method name runs both concrete invocations.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/annotations>` ·
   :download:`Complete runnable source <../../../examples/example08_typed.cpp>`
