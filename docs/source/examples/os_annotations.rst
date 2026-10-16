Skip platform-specific tests
============================

.. container:: guide-intro

   Skip a case when its behavior is only applicable on particular operating systems.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Declare the supported systems
-----------------------------

Attach ``requires_os`` to allow only the listed systems, or ``disallows_os`` to exclude them.

.. literalinclude:: ../../../examples/example04_os_annotations.cpp
   :language: cpp
   :lines: 9-12

Keep the body portable
----------------------

Keep the body compilable on every build platform. An execution filter does not remove unsupported C++ from compilation.

Run the platform case
---------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example4_os_annotations
   ./build/docs-examples/examples/example4_os_annotations --test-name PlatformTests.linux_only

.. admonition:: Expected result
   :class: guide-result

   On Linux the selected case passes. Elsewhere it is reported as skipped. Several entries in ``requires_os`` allow any one of the listed systems.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/annotations>` ·
   :download:`Complete runnable source <../../../examples/example04_os_annotations.cpp>`
