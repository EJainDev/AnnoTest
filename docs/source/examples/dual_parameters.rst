Pair types with input values
============================

.. container:: guide-intro

   Supply different input values for different type specializations of one test.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Pair each type with its values
------------------------------

Define a ``parameterize_pair`` for each template argument set and its matching function arguments.

.. literalinclude:: ../../../examples/example09_dual_parameters.cpp
   :language: cpp
   :lines: 7-11

Annotate the method template
----------------------------

Apply ``parameterize_dual{integers, floating}`` to a method with ``template <typename T = int>`` and arguments ``(T input, T expected)``.

Run both specializations
------------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example9_dual_parameters
   ./build/docs-examples/examples/example9_dual_parameters --test-name DualParameterTests.double_value

.. admonition:: Expected result
   :class: guide-result

   The integer invocation checks 7 + 7 against 14; the ``double`` invocation checks 2.5 + 2.5 against 5.0.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/annotations>` ·
   :download:`Complete runnable source <../../../examples/example09_dual_parameters.cpp>`
