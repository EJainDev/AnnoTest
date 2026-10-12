Execution and extension APIs
============================

Fixtures and suites
-------------------

``run_tests<SuiteA, SuiteB>(argc, argv)`` discovers multiple suites. A fresh fixture
is constructed for each invocation, including every parameterized case. To share
state explicitly, annotate a suite with
``suite{.lifecycle = fixture_lifecycle::shared}``. Constructors and destructors
manage fixture resources.

``nested{}`` creates a child suite with the outer name prefix and tags. Enclosing
fixtures are constructed and their setup hooks run before inner fixtures, even
when only one inner case is selected. An inner constructor may accept ``Parent&``
or ``Parent&, test_context&``. Teardown unwinds the chain in reverse order.
Public annotated base methods are inherited unless hidden by derived methods.
Multiple hooks run base-first for setup and in reverse order for cleanup;
cleanup continues after a hook fails. Duplicate annotations on one method are rejected. Composed annotations
are marker types carrying their own test/tag annotations.

``skip(reason)`` and ``assume(condition, reason)`` abort the body without a failure;
cleanup still runs. A cleanup failure takes precedence over a skip.

Selection and execution
-----------------------

``tag<"name">{}`` labels suites or methods. ``--filter`` and ``--exclude`` match
fully qualified names using ``*`` and ``?``; ``--tag`` and ``--exclude-tag`` select
labels. All four options may be repeated. ``--run-disabled`` overrides disabling.

``--repeat N``, ``--shuffle --seed N`` and ``--fail-fast`` control execution.
``test{.repetitions = N}`` repeats each concrete case within each CLI repetition.
The context's ``repetition`` index is zero-based across the combined invocations;
reports store its one-based value in the ``repetition`` property. Shared fixtures
retain their state; ordinary fixtures are fresh on each invocation. Fail-fast
stops subsequent repetitions after a failure. Isolated workers execute exactly
one invocation each.
``--shard-count N --shard-index I`` creates deterministic, disjoint partitions
using full names. Indices start at zero. Repetitions reuse shared fixtures.

``enabled_if`` and ``disabled_if`` evaluate runtime predicates before fixture
construction and extension setup; see :doc:`annotations`. Skips retain a reason
in XML/JSON and listener results. Ordinary conditions are evaluated in the worker
when execution is isolated, so they observe the worker's environment and the
invocation's repetition index.

``--isolate`` re-executes each case in a subprocess. ``--jobs N`` schedules up to N
workers. ``timeout{seconds}`` or ``--timeout SECONDS`` enforces a hard deadline
including worker startup and kills the worker process tree. Crashes and timeouts
become failed cases; later cases continue unless fail-fast is active. Results are
reported in scheduling order. Active batches complete before fail-fast takes effect.

Runtime sources and context
---------------------------

``parameter_source<&provider>{}`` on a test uses a provider returning a range of
tuples or ``case_of("name", args...)`` objects. ``csv_cases(path, header)`` parses
quoted CSV fields into named string-column cases. ``line_cases(path)`` produces
named string cases. Providers run during discovery and must return stable data
across worker invocations. Empty/duplicate case sets fail discovery.

``test_factory{}`` on a method returning ``dynamic_case{name, callable}`` values
creates a factory fixture. Its setup runs once and its state is preserved across
children; cleanup runs once after the children. ``test_factory<&provider>{}``
can also reference a free or member provider. Factories run during runtime discovery
so generated names can be listed. They must produce deterministic names. Isolated
workers reconstruct a factory for their selected case.
A test method or fixture constructor may receive ``test_context&``. The context
contains suite/name/tags/repetition/parameters and a lazily created ``temp_dir()``
that is removed after execution. ``current_test()`` accesses the active context.
Shared fixtures own their constructor context until destruction; name, tag and
repetition fields are refreshed before each case. ``context.resources`` provides
an explicit ``get_or_create<T>(key, factory)`` RAII store. Keys enforce type identity
and resources are released in reverse registration order.

Diagnostics and matchers
------------------------

``scoped_trace`` and ``scoped_message`` add user context to failures. Comparisons
provide first-difference diagnostics for ranges. Values lacking a formatter use
stream printing or a type-name fallback. ``fail`` is fatal;
``add_failure_message`` records a nonfatal failure.

``assert_that`` and ``expect_that`` accept matchers from ``annotest``. These include
comparison, string/regex, range, field/property, pointer, optional/variant,
exception and numeric tolerance checks. ``all_of``, ``any_of`` and ``negate``
compose matchers; ``satisfies`` wraps a custom predicate. Unordered range matching
uses a one-to-one matching algorithm, including overlapping matchers.

Listeners and reports
---------------------

``add_listener(shared_ptr<test_listener>, optional_suite_prefix)`` registers discovery, suite/test,
assertion, result and run-completion callbacks. ``before_test``/``after_test``
provide extension hooks around fixture construction and teardown. Override
``on_run_end`` to implement a custom reporter. Clear registrations with
``clear_listeners()``. In isolated runs, extension hooks execute in workers and
assertion events with source locations and result callbacks execute in the parent.
Only entered extension setup callbacks are unwound. Observer exceptions become
separate failure entries without discarding finished tests or their XML report.

``record_property(name, value)`` adds test metadata to XML. Reports contain one
case per skip, with a reason, and failure phase attributes. Multiple suites have
a ``testsuites`` root with aggregate totals; single-suite reports retain the
``testsuite`` root. Execution and output capture are process-wide; simultaneous
or recursive in-process runners are rejected. Use subprocess workers for parallelism.

Runtime registration
--------------------

``register_test("suite", "name", callable, tags)`` registers a callable before
execution. ``run_registered_tests(argc, argv)`` runs registered tests alone;
``run_tests<Suite>()`` combines them with annotated suites. Clear registrations
with ``clear_registered_tests()``.

``register_test<Fixture>(suite, name, factory, body, tags)`` constructs a fixture
with the supplied factory and calls ``body(Fixture&)`` between its annotated
hooks. Factories return ``std::unique_ptr<Fixture>`` and may accept
``test_context&``. Registrations carry the caller's source location. Repeat the
same registration in each worker invocation when using subprocess execution.

Reporter data and output options
--------------------------------

``test_result`` includes individual failure details, properties, captured stdout
and stderr, source location, parameters, and tags. ``on_test_discovered`` receives
``test_info``; ``on_suite_result`` receives final suite counts and timing.

``--json-output FILE`` writes structured results with suite/test metadata,
statuses, timings, failure diagnostics, skip reasons, properties, and output.
It can be combined with ``--xml-output``. Malformed captured bytes use visible
hexadecimal escapes. ``--brief`` prints failed cases and a run summary.
``--break-on-failure`` triggers a debugger breakpoint when a failure is recorded.

Environment defaults
--------------------

Options can be supplied through ``ANNOTEST_FILTER``, ``ANNOTEST_EXCLUDE``,
``ANNOTEST_TAG``, ``ANNOTEST_EXCLUDE_TAG``, ``ANNOTEST_XML_OUTPUT``,
``ANNOTEST_JSON_OUTPUT``, ``ANNOTEST_REPEAT``, ``ANNOTEST_SEED``, ``ANNOTEST_JOBS``,
``ANNOTEST_TIMEOUT``, ``ANNOTEST_SHARD_COUNT``, and ``ANNOTEST_SHARD_INDEX``.
Boolean options use ``0`` or ``1``: ``ANNOTEST_BRIEF``,
``ANNOTEST_BREAK_ON_FAILURE``, ``ANNOTEST_SHUFFLE``, ``ANNOTEST_FAIL_FAST``,
``ANNOTEST_RUN_DISABLED``, and ``ANNOTEST_ISOLATE``.

Environment options are parsed before command-line options. Scalar command-line
values override defaults; repeatable filters and tags accumulate. Boolean
command-line flags also accept ``=0`` and ``=1``.
