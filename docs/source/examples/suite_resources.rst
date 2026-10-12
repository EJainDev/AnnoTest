Suite Resources and Nested Fixtures
===================================

Static suite hooks prepare a shared file while each test receives a fresh
fixture. The nested suite sees the enclosing fixture after its per-test setup.
Suite contexts retain their resources and temporary directory through cleanup.

.. literalinclude:: ../../../examples/example13_suite_resources.cpp
   :language: cpp

Build and run ``example13_suite_resources``.
