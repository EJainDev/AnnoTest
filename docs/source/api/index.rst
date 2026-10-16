Reference
=========

Import the public module with ``import annotest;``. Unless stated otherwise,
declarations belong to namespace ``annotest``. Each entry specifies its call
syntax, inputs, result, constraints, and observable failure behavior.

.. list-table:: Common symbols
   :header-rows: 1
   :widths: 40 60

   * - Purpose
     - Declaration
   * - Discover and run suites
     - :cpp:func:`annotest::run_tests`
   * - Mark tests and configure fixtures
     - :cpp:struct:`annotest::test` · :cpp:struct:`annotest::suite`
   * - Compare values
     - :cpp:func:`annotest::assert_equal` · :cpp:func:`annotest::expect_equal`
   * - Compose checks
     - :cpp:func:`annotest::assert_that` · :cpp:func:`annotest::all_of`
   * - Supply test inputs
     - :cpp:struct:`annotest::parameterize` · :cpp:struct:`annotest::parameter_source`
   * - Access the active case
     - :cpp:func:`annotest::current_test` · :cpp:struct:`annotest::test_context`
   * - Extend execution or reporting
     - :cpp:struct:`annotest::test_listener` · :cpp:struct:`annotest::test_result`

.. list-table:: Find a declaration
   :header-rows: 1
   :widths: 40 60

   * - Area
     - Includes
   * - :doc:`testing/annotations`
     - Tests, suites, lifecycle hooks, parameters, conditions, names, and tags.
   * - :doc:`testing/assertions`
     - Fatal assertions, nonfatal expectations, failure inspection, and groups.
   * - :doc:`testing/matchers`
     - Matcher construction, composition, and custom predicates.
   * - :doc:`testing/context`
     - Case metadata, temporary directories, resources, and report properties.
   * - :doc:`testing/sources`
     - Runtime input cases, CSV/line readers, and dynamic cases.
   * - :doc:`testing/execution`
     - Runner entry points, registration, skipping, and helper threads.
   * - :doc:`testing/listeners`
     - Observer callbacks and extension setup/cleanup.
   * - :doc:`testing/results`
     - Result records, discovery metadata, and failure categories.
   * - :doc:`testing/death`
     - Process termination, exit predicates, and stderr matching.
   * - :doc:`testing/contracts`
     - Contract checks and handler integration.
   * - :doc:`testing/command_line`
     - Options, environment variables, exit status, and report formats.

Use :ref:`genindex` for an alphabetical lookup. Procedures are in
:doc:`/examples/index`; the execution model is in :doc:`/explanation/index`.

.. toctree::
   :hidden:
   :maxdepth: 1

   testing/annotations
   testing/assertions
   testing/matchers
   testing/context
   testing/sources
   testing/execution
   testing/listeners
   testing/results
   testing/death
   testing/contracts
   testing/command_line
   /performance
   /genindex
