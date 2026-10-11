Parameterized Tests
===================

Raw Source
----------

.. literalinclude:: ../../../examples/example03_parameterized.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **parameterize with single parameters**

.. code-block:: cpp

   [[= Test{}, = parameterize{tuple(2, 3), tuple(5, 7), tuple(-1, -3)}]]
   void addition(int a, int b) {
     assert_equal(a + b, a + b);
     assert_true(a != 0 && b != 0);
   }

Each ``tuple()`` call is a complete parameter set. The parameters are injected **in order** into the test function's arguments. Three sub-runs: ``(2,3)``, ``(5,7)``, and ``(-1,-3)``.

Section 2 — **parameterize with multiple parameters**

.. code-block:: cpp

   [[= Test{}, = parameterize{tuple(1.0), tuple(2.5), tuple(0.001)}]]
   void floating_point(double x) {
     assert_near(x, x);
     assert_true(x >= 0.0);
   }

Each tuple creates one sub-run with a single value.

Section 3 — **Parameterized tests with hooks**

.. code-block:: cpp

   [[= before_each{}]] void reset() { set_count = 0; }

   [[= Test{}, = parameterize{tuple(100), tuple(200), tuple(300)}]]
   void counting(int val) {
     ++set_count;
     assert_equal(val, val);
     assert_true(val % 100 == 0);
   }

before_each runs before **each parameter set**, not each test. after_each runs after each set too. The counter resets to 0 before each of the 3 parameter sets.
