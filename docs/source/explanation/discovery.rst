Discovery and case identity
===========================

Discovery turns a C++ declaration into one or more selectable test cases. The
method, concrete case, and invocation are different units: one parameterized
method has several cases, and repetition executes each case more than once.

Reflection and inheritance
--------------------------

Compile-time reflection finds public annotated methods and lifecycle hooks.
Public base methods participate unless a derived declaration hides them.
Unannotated helpers remain ordinary C++ methods. A composed annotation is a
marker type that itself carries test or tag annotations; discovery expands
those markers before interpreting the test's metadata.

Names and selectors
-------------------

The full name combines the suite and case names. Nested suites add enclosing
prefixes. Static value parameterization adds one-based suffixes; runtime input
providers can supply names. The base method name selects all of its concrete
cases, while a concrete name selects one case.

Names must remain unique and stable. They appear in CTest entries, report
records, filters, shards, and worker arguments. A worker reconstructs discovery
before locating its selected case, so a provider that generates different names
in each process can prevent that case from being found.

Listing can perform work
------------------------

Listing ordinary tests does not construct their fixtures or execute bodies and
runtime conditions. Runtime sources still enumerate their inputs. Dynamic
factories need a fixture to produce child cases, so factory discovery can enter
suite hooks, evaluate conditions, construct a fixture, and run its setup.

A factory disabled by its condition is represented as one skipped factory
entry. Factory inputs, conditions, and names therefore need to be repeatable
both during listing and during worker reconstruction.

Selection and repetition
------------------------

Name selection, include filters, tags, and sharding restrict the candidate set;
exclusions remove cases from it. Repetition runs selected concrete cases without
changing their discovery identity. The context's repetition field distinguishes
invocations and starts at zero; reports expose a one-based repetition property.

See :doc:`/api/testing/annotations` for annotation syntax and constraints,
:doc:`/api/testing/sources` for provider values, and
:doc:`/api/testing/command_line` for selection rules.
