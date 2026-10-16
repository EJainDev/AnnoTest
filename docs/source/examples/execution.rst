Run workers with deadlines
==========================

.. container:: guide-intro

   Run a tagged subset in worker processes with a hard per-case deadline.

.. container:: guide-prerequisites

   **Before you start** · An existing test project, or the :doc:`bundled-example build <build>`.

Label the work
--------------

Attach a tag to the cases in the group. Set per-test repetitions when each selected method needs repeated execution.

.. literalinclude:: ../../../examples/example15_execution.cpp
   :language: cpp
   :lines: 7-10

Set worker limits
-----------------

Inspect ``--list-json`` metadata, then select the fast group with ``--tag`` fast. Set ``--jobs`` for worker concurrency and ``--timeout`` for the deadline in seconds.

Run the selected group
----------------------

Build the target, then run the command below:

.. code-block:: sh

   cmake --build build/docs-examples --target example15_execution
   ./build/docs-examples/examples/example15_execution --tag fast --jobs 2 --timeout 5 --json-output results.json

.. admonition:: Expected result
   :class: guide-result

   The passing method runs twice and the JSON report records the invocations. To reproduce a shuffled order, add ``--shuffle`` ``--seed`` 42. Use ``--shard-count`` and ``--shard-index`` to partition a larger suite.

.. container:: guide-related

   **Keep going** · :doc:`API details </api/testing/command_line>` ·
   :download:`Complete runnable source <../../../examples/example15_execution.cpp>`
