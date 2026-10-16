Select cases and inspect a test report
======================================

.. container:: lesson-intro

   **LESSON 5 OF 5** · Continue from :doc:`diagnostics`.

   Label the basket suite, attach report metadata, and inspect the results of an isolated run.

1. Label the suite
------------------

Change the ``BasketTests`` declaration, keeping its members:

.. literalinclude:: ../../../examples/tutorials/05_reports.cpp
   :language: cpp
   :lines: 18-18

All cases inherit this tag. Add a report property to the setup hook:

.. literalinclude:: ../../../examples/tutorials/05_reports.cpp
   :language: cpp
   :lines: 21-25

2. Inspect the active context
-----------------------------

Add one final case inside the fixture:

.. literalinclude:: ../../../examples/tutorials/05_reports.cpp
   :language: cpp
   :lines: 65-69

The runner injects the context. Its temporary directory exists during the
case and is removed during context cleanup. The property stores the basket's
total as text for the report.

3. Select the discount cases
----------------------------

Build, then filter the fully qualified names:

.. code-block:: sh

   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test --tag basket --filter '*discounts*'

.. admonition:: Check your result
   :class: lesson-result

   **Three cases pass.** The filter selects the discount rows, and the inherited
   tag admits the same cases.

4. Write reports from workers
-----------------------------

Run the whole tagged suite with two worker processes and a deadline:

.. code-block:: sh

   ./build/first-test/build/first_test --tag basket --jobs 2 --timeout 10 \
     --json-output build/first-test/results.json \
     --xml-output build/first-test/results.xml

This run reports **eight passing cases**. Both files describe those invocations,
including the properties and source locations. Each worker reconstructs its
own fixture; it does not share the basket with another process.

5. Check the report contents
----------------------------

Use Python to inspect the XML without depending on its indentation:

.. code-block:: sh

   python3 - <<'PY'
   import xml.etree.ElementTree as ET
   root = ET.parse("build/first-test/results.xml").getroot()
   cases = list(root.iter("testcase"))
   print("cases:", len(cases))
   properties = {(p.get("name"), p.get("value"))
                 for p in root.iter("property")}
   print("basket total recorded:", ("basket_total", "350") in properties)
   PY

.. code-block:: text

   cases: 8
   basket total recorded: True

.. container:: lesson-checkpoint

   **Checkpoint** · :download:`Complete main.cpp <../../../examples/tutorials/05_reports.cpp>`.
   Keep the CMake project from lesson 1; only ``main.cpp`` changes.

The tutorial project now covers fixture state, parameterized cases, matchers,
captured failures, selection, and reporting. Choose a :doc:`how-to guide </examples/index>`
for the next task, or use :doc:`/api/index` for exact declarations and options.
