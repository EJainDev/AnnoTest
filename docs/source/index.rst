AnnoTest
========

.. container:: annotest-hero

   .. image:: _static/annotest-banner.svg
      :alt: AnnoTest — annotation-based testing for C++26
      :width: 100%

.. container:: annotest-intro

   Write tests as ordinary C++ methods. Mark them with annotations, import one
   module, and let compile-time reflection discover the cases.

.. raw:: html

   <nav class="annotest-cards" aria-label="Documentation shortcuts">
     <a class="annotest-card" href="tutorials/index.html"><strong>Tutorials →</strong><span>Learn the workflow by building and running your first test.</span></a>
     <a class="annotest-card" href="examples/index.html"><strong>How-to guides →</strong><span>Solve a testing task with practical steps and focused examples.</span></a>
     <a class="annotest-card" href="api/index.html"><strong>API reference →</strong><span>Look up annotations, assertions, execution, and command-line options.</span></a>
     <a class="annotest-card" href="explanation/index.html"><strong>Explanation →</strong><span>Understand discovery, fixture lifetimes, and process isolation.</span></a>
   </nav>

A complete test
---------------

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

The runner discovers ``addition`` at compile time. Pass ``--list`` to list cases,
or ``--test-name ArithmeticTests.addition`` to run this case explicitly.

Built for real test suites
--------------------------

* **Fixtures and parameters.** Fresh or shared fixtures, lifecycle hooks, nested
  suites, and value/type parameterization.
* **Checks with context.** Fatal assertions, nonfatal expectations, matchers,
  exception checks, contracts, and death tests.
* **Execution and reporting.** Filters, tags, repetitions, subprocess workers,
  deadlines, captured output, and XML/JSON reports.

Use GCC 16 or newer with C++26 reflection, CMake 4.3.3 or newer, and Ninja.
See :doc:`installation` for platform setup, :doc:`testing` for library validation,
and :doc:`performance` for compile-time benchmarks.

.. toctree::
   :hidden:
   :maxdepth: 2
   :caption: Documentation

   tutorials/index
   examples/index
   api/index
   explanation/index
   genindex
