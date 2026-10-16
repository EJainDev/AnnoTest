Share a test file
=================

Prepare a file once for a suite while each test keeps independent fixture state.
See :ref:`howto-examples-build` for the example build setup.

1. Create the shared file in a static ``before_all`` hook using the suite context. Retain its path until ``after_all`` finishes.

.. literalinclude:: ../../../examples/example13_suite_resources.cpp
   :language: cpp
   :lines: 6-16

2. Use ``context.temp_dir()`` for the file location and release retained handles in ``after_all``. Keep per-test state on the fresh fixture rather than in the shared resource.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example13_suite_resources
   ./build/docs-examples/examples/example13_suite_resources

The repeated case starts with fresh state each time. Both it and the nested reader can access ``input.txt``. Suite cleanup checks the file before context cleanup removes the directory.

See :doc:`/api/testing/context` for API details.
:download:`Runnable source <../../../examples/example13_suite_resources.cpp>`.
