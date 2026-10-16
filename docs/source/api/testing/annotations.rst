Testing Annotations and Functions
=================================

All public API names below are in ``annotest``. Annotation names are lowercase.

.. index::
   single: test
   single: suite
   single: run_tests
   single: disabled tests
   single: test names
   single: repetitions

Tests and suites
----------------

``test{}`` marks a public method. ``test<"name">{}`` supplies a custom display
name; ``test{.disabled = true}`` skips it. ``suite<"name">{}`` names a suite,
and ``suite{.disabled = true}`` disables all its cases.
``test{.repetitions = 3}`` runs each concrete case three times; the default is 1.
Counts must be between 1 and 1,000,000. Each repetition has its own result and
follows the suite's fixture lifecycle. ``--repeat N`` multiplies this count;
the combined count must not exceed 1,000,000. Discovery lists each case once.

``run_tests<T>(argc, argv)`` discovers the annotations, constructs a suite,
runs the selected cases, and returns 0 on success or 1 on failure. See
:doc:`/usage/getting_started` for filtering, reporting, and CTest integration.

.. index::
   single: before_each
   single: after_each
   single: before_all
   single: after_all
   single: fixture lifecycle
   single: setup hooks
   single: cleanup hooks

Lifecycle
---------

``before_each{}`` and ``after_each{}`` mark setup and cleanup. Multiple hooks
are supported, including inherited public hooks. Setup runs base-first and
cleanup runs in reverse order, continuing if a cleanup hook fails. Hooks must
be publicly callable and may accept ``test_context&``. They cannot also be tests;
duplicate annotations on one hook are rejected. Hook failures fail the current
case; cleanup still runs after setup or body failure.

``before_all{}`` and ``after_all{}`` mark public static suite hooks. They accept
no required arguments or a ``test_context&``. Suite setup runs base-first once
before the first participating fixture is constructed; cleanup runs in reverse
order after the suite's fixtures are destroyed. Hooks cannot share a method with
another lifecycle annotation or a test annotation. A suite-hook context and its
resources remain alive from setup through cleanup, with ``name == "_suite"``.
Properties recorded during suite setup are copied to each participating case.

A failed or skipped suite setup prevents that suite's bodies from running;
subsequent cases receive the setup failure or skip reason. Cleanup still runs
for every suite whose setup was entered, including after fail-fast. A cleanup
failure is reported as a separate teardown case in ordinary runs and merged into
the selected case in an isolated worker. Every cleanup hook runs even if an
earlier cleanup hook throws. Enclosing suite setup precedes nested suite setup;
nested suite cleanup precedes enclosing cleanup. Each parameterized suite
instance owns its own hook context. Static user state remains shared by the C++
type, so keep instance-specific resources in the hook context.

Ordinary listing, filtered-out cases, and disabled/OS-skipped cases do not enter
suite hooks. Factories may enter them during discovery because constructing
factory fixtures requires suite resources. Each isolated worker runs its own
suite setup and cleanup; resources are not shared between processes. Suite hooks
run once across in-process repetitions, while ``before_each``/``after_each``
still wrap each invocation. See :doc:`/examples/suite_resources`.

Fixtures are fresh for each test and parameterized invocation by default.
``suite{.lifecycle = fixture_lifecycle::shared}`` retains the fixture across cases.
Constructors and destructors manage fixture resources. See :doc:`execution` for
nested fixtures, dynamic factories, context resources, and extension callbacks.

.. index::
   single: parameterize
   single: parameters
   single: parameterize_template
   single: template_params
   single: parameterize_pair
   single: parameterize_dual
   single: parameterized tests
   single: typed tests

Parameterization
----------------

``parameters(values...)`` describes one complete set of function arguments.
``parameterize{parameters(...), parameters(...)}`` expands a test into cases
with one-based suffixes. ``parameters(...).cross(parameters(...))`` produces
value combinations. See :doc:`/examples/parameterized`.

``template_params<Types...>`` describes a template argument set, and
``parameterize_template<template_params<...>, ...>{}`` applies it to a test
method template. ``parameterize_template::of`` and ``template_params::cross_t``
support type products. See :doc:`/examples/typed`.

Annotated template methods must accept an empty template argument list. Supply
default template arguments, such as ``template<typename T = int>``, as needed.
Each parameterized case uses its specified types. Unannotated helper templates
without defaults are ignored.

``parameterize_pair<template_params<...>, parameters<...>>`` couples template
and function arguments. ``parameterize_dual`` expands those pairs into cases.
See :doc:`/examples/dual_parameters`.

.. index::
   single: requires_os
   single: disallows_os
   single: operating_system
   single: operating system filters

Operating systems
-----------------

``operating_system`` has values ``linux_os``, ``macos``, ``windows``, and ``unknown``.
``requires_os{operating_system::linux_os, operating_system::macos}`` runs a test when any listed OS matches.
``disallows_os{operating_system::windows}`` skips a test when any listed OS matches. These
annotations control execution; the body must compile on every target platform.
See :doc:`/examples/os_annotations`.

.. index::
   single: enabled_if
   single: disabled_if
   single: conditional execution

Execution conditions
--------------------

``enabled_if<&predicate>{"reason"}`` runs a test only when the predicate returns
true. ``disabled_if<&predicate>{"reason"}`` skips it when the predicate returns
true. Conditions may annotate methods or suites; suite conditions apply to
nested suites and public derived suites. Multiple conditions run in declaration
order and stop at the first disabling result, with inherited suite conditions
evaluated before method conditions.

Predicates accept no arguments or ``const test_context&`` and return a value
convertible to bool. They run before fixture construction and extension setup,
once per executed repetition. Disabled and OS-skipped tests bypass conditions;
``--run-disabled`` still honors conditions. A condition that disables execution
produces a skipped result with its reason. An exception produces a failure in the
``condition`` phase; ``skip(reason)`` propagates as a skip. Reason strings may
contain up to 255 bytes; an empty reason uses the runner's default message.

.. code-block:: cpp

    bool gpu_available();

    struct Tests {
        [[= test{.repetitions = 3},
          = enabled_if<&gpu_available>{"GPU unavailable"}]]
        void gpu_check() { /* ... */ }
    };

Listing ordinary tests does not evaluate conditions. Runtime parameter providers
still enumerate their cases during discovery. Dynamic factories must evaluate
conditions before constructing their fixture during discovery; a disabled factory
is listed and reported as one skipped factory entry. Factory conditions must
therefore give consistent results during discovery and worker re-execution.

.. index::
   single: name_generator
   single: suite_name_generator
   single: readable_name
   single: parameterized suites

Naming and suite parameters
---------------------------

``name_generator<&function>{}`` on a suite generates its test names at compile
time. A method-level generator overrides the suite convention.
``suite_name_generator<&function>{}`` generates the class name used by the runner.
Generators receive a name as ``std::string_view`` and return a string or string
view. Generated names must be nonempty, unique within their scope, and contain
no tabs or line breaks.

Suite-level ``parameterize`` supplies constructor arguments;
``parameter_source`` supplies runtime constructor cases. Suite-level
``parameterize_template`` instantiates a class template for each supplied type
list. See :doc:`/examples/named_suites`.
