How-to guides
=============

Choose a task to solve in an existing test project. For a guided first run,
start with :doc:`/usage/getting_started`. Use :doc:`/api/index` to look up
signatures and options, and :doc:`/explanation/index` to understand execution.

.. _howto-examples-build:

Build the runnable examples
---------------------------

The guides use the repository's checked examples. From a source checkout with
the :doc:`/installation` toolchain available, configure once:

.. code-block:: sh

   cmake -S . -B build/docs-examples -G Ninja \
     -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug \
     -DBUILD_EXAMPLES=ON -DBUILD_TESTS=OFF

The complete runnable source is linked from each guide.
On Windows, run from UCRT64 and append ``.exe`` to executable paths.

Write checks
------------

.. toctree::
   :maxdepth: 1

   basic
   full_suite
   matchers_and_threads
   death_tests
   contracts

Manage fixtures and inputs
--------------------------

.. toctree::
   :maxdepth: 1

   lifecycle
   suite_resources
   parameterized
   typed
   dual_parameters
   named_suites
   runtime_sources

Control execution and collect results
-------------------------------------

.. toctree::
   :maxdepth: 1

   os_annotations
   execution
   reporting

Maintain a project
------------------

.. toctree::
   :maxdepth: 1

   /installation
   /testing
