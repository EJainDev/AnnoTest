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

``run_tests`` constructs one suite instance per call. Public static and const
methods are supported too. Test methods with required arguments need a
parameterization annotation. Named tests use ``test<"custom name">{}``; suites
can use ``suite<"custom suite">{}`` and can be disabled.

.. _design_decisions:

Fixture lifecycle
-----------------

A suite must be default-constructible. Constructors are allowed; public
aggregates with member initializers are often convenient. State persists
between tests unless the setup hook resets it:

.. code-block:: cpp

    struct FixtureTests {
        int value = 0;
        [[= before_each{}]] void setup() { value = 10; }
        [[= after_each{}]] void cleanup() { value = 0; }
        [[= test{}]] void increment() { assert_equal(11, ++value); }
    };

Only one hook of each kind is allowed. Duplicate hooks and combining a hook with
``test`` on the same method are compile-time errors. Setup failure prevents the
body from running; cleanup still runs after setup or body failure. Hook exceptions
fail the test. Skipped tests do not run hooks. There are no suite-wide lifecycle
hooks. Data members may be private; annotated test and hook methods must be public.

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

Reports and platform behavior
-----------------------------

``--xml-output`` writes XML containing totals, timing, failures, and captured
stdout/stderr. Capture includes C stdio and uses temporary files to accommodate
large writes. Windows synchronizes CRT descriptors with native standard handles.
Capture is process-wide, so background writers must finish before the test ends.
Windows command-line names and report paths support UTF-8.

Stack traces use ``std::stacktrace`` when the toolchain supports it; otherwise
native backtraces are used, with raw frame addresses on Windows.
:doc:`/api/testing/death` explains the Windows replay model and POSIX isolation.

More examples
-------------

All ten runnable examples are in :doc:`/examples/index`, including typed and dual
parameterization, OS filtering, death checks, contracts, and reports.
