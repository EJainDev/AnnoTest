Turn examples into a parameter table
====================================

.. container:: lesson-intro

   **LESSON 3 OF 5** · Continue from :doc:`fixtures`.

   Add a discount operation and cover three inputs with one annotated method.

1. Add the calculation
----------------------

Continue editing ``build/first-test/main.cpp``. Add this method inside
``Basket``, after ``total()``:

.. literalinclude:: ../../../examples/tutorials/03_parameters.cpp
   :language: cpp
   :lines: 13-15

The tutorial uses valid whole-number percentages and integer cents. The seeded
basket still totals 350.

2. Add rows, not copied methods
-------------------------------

Add the following method inside ``BasketTests``, keeping both earlier tests:

.. literalinclude:: ../../../examples/tutorials/03_parameters.cpp
   :language: cpp
   :lines: 36-40

Each ``parameters(...)`` holds one complete argument row. The setup hook runs
for every concrete row, so each calculation starts from the same two prices.

3. Run the expanded suite
-------------------------

.. code-block:: sh

   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test

.. admonition:: Check your result
   :class: lesson-result

   **Five cases pass**: the two fixture cases and three discount cases.
   The expected discounted totals are 350, 315, and 280 cents.

4. Select a row
---------------

List the concrete cases, then run only the second discount row:

.. code-block:: sh

   ./build/first-test/build/first_test --list
   ./build/first-test/build/first_test --test-name BasketTests.discounts.2

That row checks a 10% discount against 315. Select the base method name to run
all three rows:

.. code-block:: sh

   ./build/first-test/build/first_test --test-name BasketTests.discounts

The base name selects three cases; its numeric suffix selects one.
The :doc:`parameterization reference </api/testing/annotations>` describes
type arguments and runtime sources for other input shapes.

.. container:: lesson-checkpoint

   **Checkpoint** · :download:`Complete main.cpp <../../../examples/tutorials/03_parameters.cpp>`.
   Keep the CMake project from lesson 1; only ``main.cpp`` changes.

Continue with :doc:`diagnostics`.
