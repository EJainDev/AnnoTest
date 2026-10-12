Named and Parameterized Suites
=============================

``name_generator<&function>{}`` on a suite supplies the naming convention for
its test methods. A method annotation overrides the suite convention.
``suite_name_generator<&function>{}`` generates the suite name. Both functions
receive ``std::string_view`` and return a string or string view usable during
constant evaluation. ``readable_name`` replaces underscores with spaces.

``parameterize{parameters(...), ...}`` on a suite supplies constructor arguments
for each suite invocation. Every invocation runs all selected methods with fresh
fixtures by default. Numeric suffixes identify the suite invocations.

``parameter_source<&provider>{}`` accepts runtime tuples or named ``case_of``
values. Named cases become suite suffixes. Constructors may receive the supplied
arguments followed by ``test_context&``.

On a class template with default template arguments,
``parameterize_template<template_params<...>, ...>{}`` selects the concrete suite
types. Invoke the default specialization with ``run_tests<Suite<>>()``. Each
specialization has a distinct suite name.

.. literalinclude:: ../../../../examples/example11_named_suites.cpp
   :language: cpp

Build and run ``example11_named_suites``. Use ``--list`` to see the generated
suite and method names; pass the complete displayed name to ``--test-name``.
