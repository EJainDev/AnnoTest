Pair types with input values
============================

Supply different input values for different type specializations of one test.
See :ref:`howto-examples-build` for the example build setup.

1. Define a ``parameterize_pair`` for each template argument set and its matching function arguments.

.. literalinclude:: ../../../examples/example09_dual_parameters.cpp
   :language: cpp
   :lines: 7-11

2. Apply ``parameterize_dual{integers, floating}`` to a method with ``template <typename T = ``int``>`` and arguments ``(T input, T expected)``.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example9_dual_parameters
   ./build/docs-examples/examples/example9_dual_parameters --test-name DualParameterTests.double_value

The integer invocation checks 7 + 7 against 14; the ``double`` invocation checks 2.5 + 2.5 against 5.0.

See :doc:`/api/testing/annotations` for API details.
:download:`Runnable source <../../../examples/example09_dual_parameters.cpp>`.
