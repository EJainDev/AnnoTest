Tutorials
=========

**Build a test project, one layer at a time.**

Follow five lessons in order. Start with one passing test, then grow the same
project into a basket suite with fixtures, parameter tables, failure diagnostics,
and machine-readable reports.

.. container:: directory-start

   **Before you start** · Use the :doc:`supported toolchain </installation>`
   and an AnnoTest checkout. Lessons 2–5 keep lesson 1's CMake project and change
   only ``main.cpp``. Each lesson includes a complete source checkpoint.

.. raw:: html

   <div class="learning-path">
     <a class="lesson-link" href="../usage/getting_started.html"><span class="lesson-number">01</span><span><small>THE FIRST RUN</small><strong>Write and run your first test</strong><span>Configure a small project, inspect a deliberate failure, and run it through CTest.</span></span><span class="lesson-arrow" aria-hidden="true">→</span></a>
     <a class="lesson-link" href="fixtures.html"><span class="lesson-number">02</span><span><small>FIXTURE STATE</small><strong>Build a fixture with predictable state</strong><span>Seed a basket before each case and check that mutation cannot leak between invocations.</span></span><span class="lesson-arrow" aria-hidden="true">→</span></a>
     <a class="lesson-link" href="parameters.html"><span class="lesson-number">03</span><span><small>INPUT TABLES</small><strong>Turn examples into a parameter table</strong><span>Check three discount calculations with one method and select an individual row.</span></span><span class="lesson-arrow" aria-hidden="true">→</span></a>
     <a class="lesson-link" href="diagnostics.html"><span class="lesson-number">04</span><span><small>FAILURE BEHAVIOR</small><strong>Inspect failures without failing the suite</strong><span>Use matchers, capture a failure, and observe fatal and nonfatal control flow.</span></span><span class="lesson-arrow" aria-hidden="true">→</span></a>
     <a class="lesson-link" href="reports.html"><span class="lesson-number">05</span><span><small>RESULTS AND WORKERS</small><strong>Select cases and inspect a test report</strong><span>Tag cases, run workers, write XML and JSON, and verify the reported metadata.</span></span><span class="lesson-arrow" aria-hidden="true">→</span></a>
   </div>

After the sequence
------------------

Choose :doc:`/examples/index` to solve a specific task.
Use :doc:`/api/index` to look up declarations and :doc:`/explanation/index`
to understand the execution model.

.. toctree::
   :hidden:
   :maxdepth: 1

   /usage/getting_started
   fixtures
   parameters
   diagnostics
   reports
