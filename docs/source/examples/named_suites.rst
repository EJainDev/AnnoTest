Test fixture configurations
===========================

Run every method in a fixture for each configuration, such as several constructor argument pairs.
See :ref:`howto-examples-build` for the example build setup.

1. Move configuration values into the constructor and attach matching ``parameterize`` tuples to the suite.

.. literalinclude:: ../../../examples/example11_named_suites.cpp
   :language: cpp
   :lines: 7-17

2. Use ``name_generator`` and ``suite_name_generator`` when readable displayed names help distinguish configurations. List the names before selecting one.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example11_named_suites
   ./build/docs-examples/examples/example11_named_suites --list

The addition suite has configurations (2, 3) and (10, 20), each with two methods. Copy a complete displayed name into ``--test-name`` to run one case. The source also includes a class-template suite.

See :doc:`/api/testing/execution` for API details.
:download:`Runnable source <../../../examples/example11_named_suites.cpp>`.
