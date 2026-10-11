Testing Annotations and Functions
=================================

All public API names below are in ``annotest``. Annotation names are lowercase.

Tests and suites
----------------

``test{}`` marks a public method. ``test<"name">{}`` supplies a custom display
name; ``test{.disabled = true}`` skips it. ``suite<"name">{}`` names a suite,
and ``suite{.disabled = true}`` disables all its cases.

``run_tests<T>(argc, argv)`` discovers the annotations, constructs a suite,
runs the selected cases, and returns 0 on success or 1 on failure. See
:doc:`/usage/getting_started` for filtering, reporting, and CTest integration.

Lifecycle
---------

``before_each{}`` and ``after_each{}`` mark setup and cleanup. Only one of each
is allowed per suite. Hooks take no required arguments and must be publicly
callable. They cannot also be tests. Hook failures fail the current case;
cleanup still runs after setup or body failure.

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
