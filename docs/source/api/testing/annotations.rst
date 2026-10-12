Testing Annotations and Functions
=================================

All public API names below are in ``annotest``. Annotation names are lowercase.

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

Lifecycle
---------

``before_each{}`` and ``after_each{}`` mark setup and cleanup. Multiple hooks
are supported, including inherited public hooks. Setup runs base-first and
cleanup runs in reverse order, continuing if a cleanup hook fails. Hooks must
be publicly callable and may accept ``test_context&``. They cannot also be tests;
duplicate annotations on one hook are rejected. Hook failures fail the current
case; cleanup still runs after setup or body failure.

Fixtures are fresh for each test and parameterized invocation by default.
``suite{.lifecycle = fixture_lifecycle::shared}`` retains the fixture across cases.
Constructors and destructors manage fixture resources. See :doc:`execution` for
nested fixtures, dynamic factories, context resources, and extension callbacks.

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

GCC exposes method-template annotations through a specialization. Annotated
template methods must accept an empty template argument list, typically by
giving their template parameters defaults, such as ``template<typename T = int>``.
The defaults enable discovery; each parameterized case still uses its specified
types. Unannotated helper templates without defaults are ignored.

``parameterize_pair<template_params<...>, parameters<...>>`` couples template
and function arguments. ``parameterize_dual`` expands those pairs into cases.
See :doc:`/examples/dual_parameters`.

Operating systems
-----------------

``OS`` has values ``Linux``, ``Mac``, ``Windows``, and ``Unknown``.
``requires_os{OS::Linux, OS::Mac}`` runs a test when any listed OS matches.
``disallows_os{OS::Windows}`` skips a test when any listed OS matches. These
annotations control execution; the body must compile on every target platform.
See :doc:`/examples/os_annotations`.

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
``--run-disabled`` still honors conditions. A false condition produces a normal
skipped result with its reason. An exception produces a failure in the
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
