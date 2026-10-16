Failures, skips, and diagnostics
================================

AnnoTest records outcomes separately from the C++ control flow used to reach
them. A case can record several failures and still execute cleanup and produce
one completed result.

Assertions and expectations
----------------------------

A fatal assertion records a failure and unwinds its current callable. It can
guard a prerequisite before later operations dereference a pointer or inspect a
result. A nonfatal expectation records a failure and continues, allowing several
independent observations of the same result. Both make the case fail.

An assertion group creates smaller callable boundaries. A fatal assertion ends
one block, allowing later blocks to run. A fatal group unwinds after its blocks
finish when they recorded a failure; a nonfatal group continues afterward.
This does not duplicate the original failures or replace their locations.

Skipping and cleanup
---------------------

A runtime skip ends execution with a reason. Cleanup still runs, and a cleanup
failure makes the final outcome failed. A case skipped before entering its
fixture does not run its per-test hooks. Suite setup failures prevent bodies
from running, but entered suite cleanup hooks still execute.

Diagnostic context
------------------

Failure records include a category, message, phase, and an optional source
location. A check's optional message describes the input or operation being
checked. A scoped trace adds context to multiple checks until its scope ends.
Traces belong to their creating thread.

Value printers run when a comparison fails. C strings need content-specific
checks because ordinary equality compares their addresses. Range comparisons
can show the first differing position; custom printers describe application
types inside those diagnostics.

Testing failure behavior
-------------------------

Failure capture redirects checks into a returned collection without failing the
outer case. It allows a testing library or a custom assertion to test the
failure category and diagnostic text it produces. The callable still unwinds
on a fatal assertion. Capturing failures does not turn unexpected exceptions
into successful checks.

The signatures and exact behavior are in :doc:`/api/testing/assertions`;
in-memory records are in :doc:`/api/testing/results`. For practical checks,
use :doc:`/examples/basic` or :doc:`/examples/matchers_and_threads`.
