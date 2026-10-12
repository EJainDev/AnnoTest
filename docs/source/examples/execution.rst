Selection, Repetition, and Parallel Execution
=============================================

Run this example with ``--list-json`` to inspect metadata, or use
``--jobs 2 --timeout 5 --repeat 2 --json-output results.json`` to run subprocess
workers. ``--tag fast`` selects tagged cases. ``--shuffle --seed 42`` reproduces
an order; ``--shard-count 2 --shard-index 0`` selects a deterministic partition.
The runtime condition skips odd invocation indices.

.. literalinclude:: ../../../examples/example15_execution.cpp
   :language: cpp

Build and run ``example15_execution``.
