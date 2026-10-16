Skip platform-specific tests
============================

Skip a case when its behavior is only applicable on particular operating systems.
See :ref:`howto-examples-build` for the example build setup.

1. Attach ``requires_os`` to allow only the listed systems, or ``disallows_os`` to exclude them.

.. literalinclude:: ../../../examples/example04_os_annotations.cpp
   :language: cpp
   :lines: 9-12

2. Keep the body compilable on every build platform. An execution filter does not remove unsupported C++ from compilation.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example4_os_annotations
   ./build/docs-examples/examples/example4_os_annotations --test-name PlatformTests.linux_only

On Linux the selected case passes. Elsewhere it is reported as skipped. Several entries in ``requires_os`` allow any one of the listed systems.

See :doc:`/api/testing/annotations` for API details.
:download:`Runnable source <../../../examples/example04_os_annotations.cpp>`.
