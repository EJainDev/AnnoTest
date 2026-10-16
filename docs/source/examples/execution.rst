Run workers with deadlines
==========================

Run a tagged subset in worker processes with a hard per-case deadline.
See :ref:`howto-examples-build` for the example build setup.

1. Attach a tag to the cases in the group. Set per-test repetitions when each selected method needs repeated execution.

.. literalinclude:: ../../../examples/example15_execution.cpp
   :language: cpp
   :lines: 7-10

2. Inspect ``--list-json`` metadata, then select the fast group with ``--tag`` fast. Set ``--jobs`` for worker concurrency and ``--timeout`` for the deadline in seconds.

3. Build the target and run the selected case or group:

.. code-block:: sh

   cmake --build build/docs-examples --target example15_execution
   ./build/docs-examples/examples/example15_execution --tag fast --jobs 2 --timeout 5 --json-output results.json

The passing method runs twice and the JSON report records the invocations. To reproduce a shuffled order, add ``--shuffle`` ``--seed`` 42. Use ``--shard-count`` and ``--shard-index`` to partition a larger suite.

See :doc:`/api/testing/command_line` for API details.
:download:`Runnable source <../../../examples/example15_execution.cpp>`.
