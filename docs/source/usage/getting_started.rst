.. _getting_started:

Getting Started
===============

.. _testing_example:

Write and run a suite
---------------------

AnnoTest is a C++26 module. Import it, use the ``annotest`` namespace, annotate
public methods with ``test``, and forward the command line to ``run_tests``:

.. code-block:: cpp

    import annotest;
    using namespace annotest;

    struct AddTests {
        [[= test{}]] void basic() { assert_equal(4, 2 + 2); }
        [[= test{.disabled = true}]] void skipped() {}
        [[= test{}, = parameterize{parameters(5), parameters(50)}]]
        void parameterized(int value) { assert_true(value == 5 || value == 50); }
    };

    int main(int argc, char** argv) {
        return run_tests<AddTests>(argc, argv);
    }

``run_tests`` constructs a fresh fixture for each test invocation by default.
Public static and const methods are supported too. Test methods may receive
``test_context&``; other required arguments need a parameterization annotation.
Inaccessible or deleted annotated methods, missing
arguments, empty parameterizations, duplicate names, and names with tabs or line
breaks are compile-time errors. Private unannotated helpers remain allowed.
Named tests use ``test<"custom name">{}``; suites
can use ``suite<"custom suite">{}`` and can be disabled.

Fixture lifecycle
-----------------

A suite must be default-constructible or accept ``test_context&``. A fresh
instance is constructed for every test and parameterized invocation. Constructors
and destructors manage fixture resources. Use
``suite{.lifecycle = fixture_lifecycle::shared}`` to retain state explicitly.
This example uses the default fresh fixture:

.. code-block:: cpp

    struct FixtureTests {
        int value = 0;
        [[= before_each{}]] void setup() { value = 10; }
        [[= after_each{}]] void cleanup() { value = 0; }
        [[= test{}]] void increment() { assert_equal(11, ++value); }
    };

Multiple hooks are allowed: setup runs base-first and cleanup runs in reverse
order. Duplicate annotations and combining a hook with ``test`` on the same
method are compile-time errors. Setup failure prevents the
body from running; cleanup still runs after setup or body failure. Hook exceptions
fail the test, and later cleanup hooks still run. Tests disabled or excluded by
OS requirements do not run hooks; runtime skips still run cleanup.
Data members may be private; annotated test and hook methods must be public.

A failed assertion records a failure and unwinds the current test body. A failed
expectation records a failure and continues. Both make the run return 1, and later
tests still execute. Uncaught exceptions and unexpected contract violations also
fail tests. See :doc:`/api/testing/assertions` and :doc:`/api/testing/contracts`.

Command line and CTest
----------------------

.. code-block:: sh

    ./my_tests --list
    ./my_tests --test-name AddTests.basic
    ./my_tests --test-name AddTests.parameterized.2 --xml-output report.xml

Windows executables have an ``.exe`` suffix. Filters use fully qualified names;
parameterized cases have one-based numeric suffixes. ``--help`` describes the
supported options. Test failures, invalid arguments, unknown names, and report
write failures return 1; successful runs return 0.

``annotest_discover_tests(my_tests)`` lists the executable after building it and
registers each case with CTest. Each selected case runs in its own process, with
its own suite instance and per-test hooks. Running the executable directly
runs the suite in a single process.

Discovery fails the build if listing fails, times out, is malformed, contains
no cases, or repeats a fully qualified name. CTest fails visibly if the executable
or discovery manifest is missing, with an instruction to build the target.
Quotes, semicolons, brackets, dollar signs, and surrounding spaces in names are
preserved. ``DISCOVERY_TIMEOUT`` accepts a positive integer in seconds:

.. code-block:: cmake

    annotest_discover_tests(my_tests DISCOVERY_TIMEOUT 60)

The default timeout is 30 seconds. Multi-config generators use separate manifests;
select a configuration with ``ctest -C Debug`` or ``ctest -C Release``.

Reports and platform behavior
-----------------------------

``--xml-output`` writes XML containing totals, timing, failures, and captured
stdout/stderr. Capture includes C stdio and uses temporary files to accommodate
large writes. Windows synchronizes CRT descriptors with native standard handles.
Capture is process-wide, so background writers must finish before the test ends.
Windows command-line names and report paths support UTF-8.

XML preserves valid Unicode. Forbidden XML control bytes and malformed UTF-8
are represented as visible ``\xNN`` escapes in the report. Captured runtime
data and console diagnostics retain their original bytes.

Stack traces use ``std::stacktrace`` when the toolchain supports it; otherwise
native backtraces are used, with raw frame addresses on Windows.
:doc:`/api/testing/death` explains the Windows replay model and POSIX isolation.

More examples
-------------

Runnable examples are in :doc:`/examples/index`, including typed and dual
parameterization, named and parameterized suites, OS filtering, death checks,
contracts, and reports.
