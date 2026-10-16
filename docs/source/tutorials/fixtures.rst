Build a fixture with predictable state
======================================

.. container:: lesson-intro

   **LESSON 2 OF 5** · Continue from :doc:`/usage/getting_started`.

   Build a basket fixture, seed it before every test, and verify that a change in one case cannot affect another.

1. Keep the same project
------------------------

Use the ``build/first-test`` project from lesson 1. Leave its CMake file and
target name unchanged. Replace the contents of ``main.cpp`` in the following
steps. Prices are integer cents, so these checks do not involve floating-point
rounding.

2. Write the object under test
------------------------------

Start ``main.cpp`` with the imports and the basket type:

.. literalinclude:: ../../../examples/tutorials/02_fixtures.cpp
   :language: cpp
   :lines: 1-12

The basket owns its prices. Its total is the sum of those values.

3. Prepare each fixture
-----------------------

Add the fixture below the basket type:

.. literalinclude:: ../../../examples/tutorials/02_fixtures.cpp
   :language: cpp
   :lines: 14-20

The runner constructs a fresh ``BasketTests`` for each invocation. Its setup
hook adds the same two prices before the body runs.

4. Add two independent cases
----------------------------

Inside ``BasketTests``, add these methods, then close the struct with ``};``:

.. literalinclude:: ../../../examples/tutorials/02_fixtures.cpp
   :language: cpp
   :lines: 22-31

Finish the file with a runner for the new fixture:

.. literalinclude:: ../../../examples/tutorials/02_fixtures.cpp
   :language: cpp
   :lines: 34-36

5. Build and check isolation
----------------------------

.. code-block:: sh

   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test

.. admonition:: Check your result
   :class: lesson-result

   The run reports **two passing cases**: ``has_initial_items`` and
   ``adding_an_item``. One case sees a total of 350 and the other sees 400.

Run them repeatedly in a shuffled order:

.. code-block:: sh

   ./build/first-test/build/first_test --shuffle --seed 42 --repeat 3

All six invocations pass. The setup never accumulates prices from an earlier
case, because each invocation starts with a fresh basket.

.. container:: lesson-checkpoint

   **Checkpoint** · :download:`Complete main.cpp <../../../examples/tutorials/02_fixtures.cpp>`.
   Keep the CMake project from lesson 1; only ``main.cpp`` changes.

Continue with :doc:`parameters`.
