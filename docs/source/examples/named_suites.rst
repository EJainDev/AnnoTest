Test fixture configurations
===========================

.. container:: guide-intro

   Run every method in a fixture for each configuration, such as several constructor argument pairs.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Parameterize the constructor
----------------------------

Move configuration values into the constructor and attach matching ``parameterize`` tuples to the suite.

.. literalinclude:: ../../../examples/example11_named_suites.cpp
   :language: cpp
   :lines: 7-17

Inspect readable names
----------------------

Use ``name_generator`` and ``suite_name_generator`` when readable displayed names help distinguish configurations. List the names before selecting one.

List the configurations
-----------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example11_named_suites
   ./build/docs-examples/examples/example11_named_suites --list

.. admonition:: Expected result
   :class: guide-result

   The addition suite has configurations (2, 3) and (10, 20), each with two methods. Copy a complete displayed name into ``--test-name`` to run one case. The source also includes a class-template suite.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/execution>` ·
   :download:`Complete runnable source <../../../examples/example11_named_suites.cpp>`
