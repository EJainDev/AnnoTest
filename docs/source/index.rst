AnnoTest
========

.. container:: annotest-hero

   .. image:: _static/AnnoTest.png
      :alt: AnnoTest — annotation-based testing for C++26
      :width: 100%

Tests are ordinary C++ methods
------------------------------

Annotate methods, import one module, and let C++26 reflection discover the tests.
Check values, manage fixtures, and collect results without test registration macros.

.. code-block:: cpp

   import annotest;
   using namespace annotest;

   struct ArithmeticTests {
       [[= test{}]] void addition() {
           assert_equal(4, 2 + 2);
       }
   };

   int main(int argc, char** argv) {
       return run_tests<ArithmeticTests>(argc, argv);
   }

:doc:`Build your first test → <usage/getting_started>` ·
:doc:`Look up the API <api/index>`

What do you want to do?
-----------------------

.. raw:: html

   <nav class="annotest-cards" aria-label="Documentation paths">
     <a class="annotest-card" href="tutorials/index.html"><small>LEARN</small><strong>Tutorials</strong><span>Build a first test project and follow a complete workflow with expected results.</span><em>Start learning →</em></a>
     <a class="annotest-card" href="examples/index.html"><small>SOLVE A TASK</small><strong>How-to guides</strong><span>Choose a fixture, supply inputs, check failures, or collect a report.</span><em>Find your task →</em></a>
     <a class="annotest-card" href="api/index.html"><small>LOOK IT UP</small><strong>Reference</strong><span>Find declarations, parameters, defaults, constraints, and failure behavior.</span><em>Check the details →</em></a>
     <a class="annotest-card" href="explanation/index.html"><small>UNDERSTAND</small><strong>Explanation</strong><span>Understand discovery, fixture lifetimes, failure control flow, and processes.</span><em>Explore the model →</em></a>
   </nav>

Common destinations
-------------------

.. list-table::
   :header-rows: 1
   :widths: 48 52

   * - I need to…
     - Go to
   * - Add AnnoTest to a CMake project
     - :doc:`installation`
   * - Find an assertion or matcher
     - :doc:`api/testing/assertions` · :doc:`api/testing/matchers`
   * - Check annotation syntax
     - :doc:`api/testing/annotations`
   * - Run selected tests or write reports
     - :doc:`api/testing/command_line`
   * - Compare compilation with GoogleTest
     - :doc:`performance`
   * - Find a symbol by name
     - :ref:`genindex`

.. note::

   The supported toolchain uses GCC 16 or newer with C++26 reflection,
   CMake 4.3.3 or newer, and Ninja. See :doc:`installation` for setup.

`Source code <https://github.com/EJainDev/AnnoTest>`_ ·
`Report an issue <https://github.com/EJainDev/AnnoTest/issues>`_ ·
:doc:`Contribute <project/index>`

.. toctree::
   :hidden:
   :maxdepth: 2

   tutorials/index
   examples/index
   api/index
   explanation/index
   project/index
