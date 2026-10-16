.. index::
   single: command-line options

Command-Line Reference
======================

Forward ``argc`` and ``argv`` to ``run_tests<Suites...>(argc, argv)`` or
``run_registered_tests(argc, argv)``. Long options with values accept either
``--option value`` or ``--option=value``. Boolean flags accept ``=0`` and ``=1``;
supplying a flag without a value enables it. Positional arguments are rejected.

.. index::
   single: help (--help)
   single: listing tests (--list)
   single: JSON discovery (--list-json)
   single: selecting tests (--test-name)
   single: filters (--filter, --exclude)
   single: tags (--tag, --exclude-tag)
   single: running disabled tests (--run-disabled)

Selection
---------

.. list-table:: Discovery and selection options
   :header-rows: 1
   :widths: 35 65

   * - Option
     - Behavior
   * - ``-h``, ``--help``
     - Prints usage and exits.
   * - ``-l``, ``--list``
     - Lists discovered suites and cases without executing ordinary test bodies.
   * - ``--list-json``
     - Lists the same discovered cases as ``--list``, with versioned JSON metadata
       on stdout. Does not accept a value.
   * - ``-t``, ``--test-name NAME...``
     - Selects fully qualified names. Accepts multiple names and may be repeated.
   * - ``--filter GLOB``
     - Includes names matching any supplied pattern. May be repeated.
   * - ``--exclude GLOB``
     - Excludes names matching any supplied pattern. May be repeated.
   * - ``--tag TAG``
     - Includes cases with any supplied tag. May be repeated.
   * - ``--exclude-tag TAG``
     - Excludes cases with any supplied tag. May be repeated.
   * - ``--run-disabled``
     - Executes annotation-disabled tests. OS restrictions and runtime conditions
       remain active. Disabled by default.

Names use ``Suite.Test``. Parameterized cases append a one-based numeric suffix
or a provider-supplied name. Selecting a parameterized method's base name selects
all its cases. Unknown explicit names fail the run. Quote names containing spaces.
Glob patterns match fully qualified names: ``*`` matches any sequence and ``?``
matches one character. Quote patterns to prevent shell expansion.

Explicit names, include filters, include tags, and sharding each restrict the
selection. Exclusions remove cases from that selection. With no selection
options, all discovered cases are selected; annotation-disabled cases are skipped.
``--list`` includes disabled cases and runs runtime providers and dynamic factories
needed to discover names. See :doc:`annotations` for discovery-time conditions.

.. code-block:: sh

    ./my_tests --test-name AddTests.basic AddTests.parameterized.2
    ./my_tests --filter 'AddTests.*' --exclude '*.slow' --tag fast

.. index::
   single: repeat (--repeat)
   single: shuffle (--shuffle)
   single: seed (--seed)
   single: fail-fast (--fail-fast)
   single: sharding (--shard-count, --shard-index)
   single: isolation (--isolate)
   single: jobs (--jobs)
   single: timeouts (--timeout)

Execution
---------

.. list-table:: Execution options
   :header-rows: 1
   :widths: 35 65

   * - Option
     - Behavior and default
   * - ``--repeat N``
     - Repeats each selected case. Range: 1 to 1,000,000. Default: 1.
   * - ``--shuffle``
     - Shuffles cases within each suite. Disabled by default.
   * - ``--seed N``
     - Sets a nonnegative shuffle seed and enables shuffling.
   * - ``--fail-fast``
     - Stops scheduling cases after a failure. Disabled by default.
   * - ``--shard-count N``
     - Sets a positive number of deterministic partitions. Default: 1.
   * - ``--shard-index I``
     - Selects a zero-based partition; ``I`` must be less than the shard count.
       Default: 0.
   * - ``--isolate``
     - Executes each invocation in a subprocess. Disabled by default.
   * - ``--jobs N``
     - Executes up to ``N`` cases concurrently in subprocesses when ``N > 1``.
       Range: 1 to 256. Default: 1.
   * - ``--timeout SECONDS``
     - Sets a positive, finite per-case subprocess deadline, including startup.
       Overrides ``timeout{seconds}``. No command-line deadline by default.

``--repeat`` multiplies ``test{.repetitions = N}``; the combined count must not
exceed 1,000,000. Each invocation has its own result. Shared fixtures retain state
across in-process repetitions; subprocesses reconstruct their fixtures.
Sharding partitions fully qualified names before repetitions are executed.

Shuffling without ``--seed`` chooses and prints a seed. Supply that seed to
reproduce the order for the same discovered cases and selection.
Crashes and timeouts produce failed cases. Results from subprocesses are reported
in scheduling order. Under fail-fast, active batches finish before scheduling stops.

.. code-block:: sh

    ./my_tests --shuffle --seed 42 --repeat 3
    ./my_tests --jobs 4 --timeout 10 --shard-count 2 --shard-index 0

.. index::
   single: XML reports (--xml-output)
   single: JSON reports (--json-output)
   single: brief output (--brief)
   single: debugger breakpoint (--break-on-failure)
   single: captured output

Reporting
---------

.. list-table:: Reporting options
   :header-rows: 1
   :widths: 35 65

   * - Option
     - Behavior
   * - ``--xml-output FILE``
     - Writes an XML report, overwriting the file. Specify once on the command line.
   * - ``--json-output FILE``
     - Writes a JSON report, overwriting the file.
   * - ``--brief``
     - Prints failed cases and a run summary. Disabled by default.
   * - ``--break-on-failure``
     - Triggers a debugger breakpoint when a failure is recorded. Disabled by default.

XML and JSON output can be enabled together. Reports include test metadata,
timings, failure details, skipped cases, properties, and captured output.
See :doc:`results` for report structures and :doc:`listeners` for callbacks.

.. code-block:: sh

    ./my_tests --xml-output report.xml --json-output report.json --brief

.. index::
   single: environment variables
   single: ANNOTEST_FILTER
   single: ANNOTEST_JOBS
   single: ANNOTEST_TIMEOUT

Environment variables
---------------------

Environment defaults are read before command-line arguments. Command-line scalar
values override their environment defaults. Include and exclude filters and tags
accumulate across both sources. Each filter or tag variable supplies one value.

.. list-table:: Environment defaults
   :header-rows: 1
   :widths: 55 45

   * - Variable
     - Corresponding option
   * - ``ANNOTEST_FILTER``, ``ANNOTEST_EXCLUDE``
     - ``--filter``, ``--exclude``
   * - ``ANNOTEST_TAG``, ``ANNOTEST_EXCLUDE_TAG``
     - ``--tag``, ``--exclude-tag``
   * - ``ANNOTEST_XML_OUTPUT``, ``ANNOTEST_JSON_OUTPUT``
     - ``--xml-output``, ``--json-output``
   * - ``ANNOTEST_REPEAT``, ``ANNOTEST_SEED``
     - ``--repeat``, ``--seed``
   * - ``ANNOTEST_JOBS``, ``ANNOTEST_TIMEOUT``
     - ``--jobs``, ``--timeout``
   * - ``ANNOTEST_SHARD_COUNT``, ``ANNOTEST_SHARD_INDEX``
     - ``--shard-count``, ``--shard-index``
   * - ``ANNOTEST_SHUFFLE``, ``ANNOTEST_FAIL_FAST``
     - ``--shuffle``, ``--fail-fast``
   * - ``ANNOTEST_RUN_DISABLED``, ``ANNOTEST_ISOLATE``
     - ``--run-disabled``, ``--isolate``
   * - ``ANNOTEST_BRIEF``, ``ANNOTEST_BREAK_ON_FAILURE``
     - ``--brief``, ``--break-on-failure``

Boolean variables accept only ``0`` and ``1``. Other variables use their option's
value format. Empty variables are ignored.

.. index::
   single: exit status

Exit status
-----------

Successful runs, successful listing, and ``--help`` return 0. Test failures,
discovery errors, unknown explicit names, invalid options, and report write failures
return 1. Skipped tests do not fail a run unless cleanup also fails.

.. index::
   single: JSON schemas
   single: discovery metadata

JSON formats
------------

Both discovery and result documents have ``schema_version: 1``. Their schemas
are :download:`discovery JSON <../../_static/discovery.schema.json>` and
:download:`report JSON <../../_static/report.schema.json>`. Consumers should ignore
unknown fields within a supported schema version and reject unsupported versions.

Discovery includes every concrete name, its fully qualified base selector, source
file/line, formatted parameters, tags, annotation-disabled status, and any static
OS skip reason. Runtime conditions remain unevaluated for ordinary tests.
Providers and factories still run to discover their children; factory lifecycle
hooks, including suite hooks, may execute. Selection options do not restrict
the listing. ``--list-json`` takes precedence if combined with ``--list``.

Result JSON includes formatted parameters and tags on each case. Every failure
has a ``location`` object containing ``file`` and ``line``, or ``null`` when no
assertion location is available. Timings are in seconds. ``properties`` remains
an array to retain duplicate user property names. Malformed output bytes use
visible hexadecimal escapes. Listing returns discovery JSON on stdout;
``--json-output`` writes execution results to a file.
