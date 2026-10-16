Share a test file
=================

.. container:: guide-intro

   Prepare a file once for a suite while each test keeps independent fixture state.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Prepare the suite resource
--------------------------

Create the shared file in a static ``before_all`` hook using the suite context. Retain its path until ``after_all`` finishes.

.. literalinclude:: ../../../examples/example13_suite_resources.cpp
   :language: cpp
   :lines: 6-16

Bound its lifetime
------------------

Use ``context.temp_dir()`` for the file location and release retained handles in ``after_all``. Keep per-test state on the fresh fixture rather than in the shared resource.

Run the readers
---------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example13_suite_resources
   ./build/docs-examples/examples/example13_suite_resources

.. admonition:: Expected result
   :class: guide-result

   The repeated case starts with fresh state each time. Both it and the nested reader can access ``input.txt``. Suite cleanup checks the file before context cleanup removes the directory.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/context>` ·
   :download:`Complete runnable source <../../../examples/example13_suite_resources.cpp>`
