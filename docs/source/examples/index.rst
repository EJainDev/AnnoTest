How-to guides
=============

Choose the outcome you need in an existing test project. Each guide contains a
focused change, a runnable command, and the result to check.

.. container:: directory-start

   **New to AnnoTest?** Follow the :doc:`tutorial sequence </tutorials/index>`.
   To run the snippets from a source checkout, :doc:`build the bundled examples <build>`.

Write checks
------------

Check values, errors, termination, and work performed on helper threads.

.. raw:: html

   <div class="task-grid">
     <a class="task-card" href="basic.html"><strong>Check values and exceptions</strong><span>Use an exception check when an operation must reject invalid input.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="full_suite.html"><strong>Test a stateful container</strong><span>Check a stack's ordering, error behavior, and round trips in independent cases.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="matchers_and_threads.html"><strong>Check helper-thread results</strong><span>Attach checks made by background work to the current test case.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="death_tests.html"><strong>Check process termination</strong><span>Check a fatal path without terminating the parent test runner.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="contracts.html"><strong>Check contract violations</strong><span>Verify a contract's invalid-input and valid-input paths under observe semantics.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
   </div>

Manage fixtures and inputs
--------------------------

Choose the lifetime of state and the source of each case's inputs.

.. raw:: html

   <div class="task-grid">
     <a class="task-card" href="lifecycle.html"><strong>Reset fixture state</strong><span>Populate fixture data before each case so test order cannot change its initial state.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="suite_resources.html"><strong>Share a test file</strong><span>Prepare a file once for a suite while each test keeps independent fixture state.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="parameterized.html"><strong>Test several input sets</strong><span>Cover several input/output pairs without copying the test body.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="typed.html"><strong>Test several types</strong><span>Apply one behavioral check to several concrete type specializations.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="dual_parameters.html"><strong>Pair types with input values</strong><span>Supply different input values for different type specializations of one test.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="named_suites.html"><strong>Test fixture configurations</strong><span>Run every method in a fixture for each configuration, such as several constructor argument pairs.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="runtime_sources.html"><strong>Load cases from runtime data</strong><span>Turn data loaded at discovery time into individually selectable cases.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
   </div>

Control execution and collect results
-------------------------------------

Select the right cases, bound execution, and inspect machine-readable results.

.. raw:: html

   <div class="task-grid">
     <a class="task-card" href="os_annotations.html"><strong>Skip platform-specific tests</strong><span>Skip a case when its behavior is only applicable on particular operating systems.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="execution.html"><strong>Run workers with deadlines</strong><span>Run a tagged subset in worker processes with a hard per-case deadline.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
     <a class="task-card" href="reporting.html"><strong>Save an XML failure report</strong><span>Write a report containing case results, failure locations, and captured output.</span><em>Open guide <span aria-hidden="true">→</span></em></a>
   </div>

Set up the environment
----------------------

:doc:`Install AnnoTest </installation>` to add the library to a project, or
:doc:`build the bundled examples <build>` to run the checked source behind these guides.

.. toctree::
   :hidden:
   :maxdepth: 1

   basic
   full_suite
   matchers_and_threads
   death_tests
   contracts
   lifecycle
   suite_resources
   parameterized
   typed
   dual_parameters
   named_suites
   runtime_sources
   os_annotations
   execution
   reporting
   build
   /installation
