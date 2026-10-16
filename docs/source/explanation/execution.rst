How a test becomes an execution
===============================

A test method, a fixture instance, and a process have different lifetimes.
Understanding those boundaries helps keep state and cleanup predictable.

.. index::
   single: test discovery

Discovery identifies cases
--------------------------

Reflection finds annotated methods at compile time. Parameterization supplies
the concrete invocations of those methods. A base method name acts as a
selection alias for its parameterized cases; each concrete case also has a
distinct displayed name.

Runtime providers and factories finish expanding cases during discovery.
Listing therefore does not execute ordinary test bodies, but it can call a
provider or construct the fixture needed for a factory. Stable discovery names
matter when a worker process later reconstructs the selected case.

.. index::
   single: fresh fixtures
   single: shared fixtures
   single: resource lifetimes

Fixtures bound mutable state
----------------------------

The default lifecycle constructs a fresh fixture for each invocation. Setup
hooks run before its body and cleanup hooks run after it, including after setup
or body failures. Repeating a case does not make its fixture shared.

A shared fixture deliberately retains state between cases within the same
runner process. That state does not cross a process boundary. Tests selected
separately by CTest, or executed in isolated workers, cannot rely on an earlier
process having initialized their fixture.

Static suite hooks and suite-context resources provide a different boundary:
they prepare resources around the suite execution while per-test fixtures
remain fresh. They run in each isolated worker's own process. They are not a
cross-process singleton.

.. index::
   single: fatal failures
   single: nonfatal failures

Failures change control flow
----------------------------

A fatal assertion records a failure and unwinds the current body. A nonfatal
expectation records a failure and permits the next statement to execute.
Independent observations can therefore use expectations, while an assertion
can guard a prerequisite for subsequent operations.

Both kinds make the run fail. Cleanup still runs, and later cases run unless a
selection or stopping option prevents them. A runtime skip also performs
cleanup; a case excluded before execution does not run its per-test hooks.

.. index::
   single: worker processes
   single: process isolation

Processes isolate execution
---------------------------

Running the executable directly normally executes its selected cases within
one runner process. CTest discovery registers separate case invocations, so
CTest starts a process for each selected case. Worker execution adds subprocess
isolation and allows hard deadlines; parent-only pointers and mutable globals
are not shared worker state.

Death checks have a narrower purpose: they run a callable that is expected to
terminate abnormally. POSIX systems use a child process, while Windows
relaunches the executable and replays the case to reach the callable.
Deterministic setup is essential for that replay.

Put the model into practice
---------------------------

Use :doc:`/examples/lifecycle` for fresh per-test data,
:doc:`/examples/suite_resources` for a shared test file, and
:doc:`/examples/execution` for workers and deadlines.
The :doc:`/api/testing/execution` and :doc:`/api/testing/death` references specify
the API and platform behavior.
