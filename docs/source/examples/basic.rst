Basic Tests
===========

Raw Source
----------

.. literalinclude:: ../../../examples/example01_basic.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **Test struct**

.. code-block:: cpp

   struct BasicTests {
     [[= Test{}]] void addition() { assert_equal(4, 2 + 2); }

     [[= Test{}]] void boolean_logic() {
       assert_true(true);
       assert_false(false);
     }

     [[= Test<"subtraction works">{}]] void math() { assert_equal(10, 15 - 5); }
   };

Each function annotated with ``[[= Test{}]]`` becomes a test case. The function name is the test display name unless a custom name is provided (see ``math``).

Section 2 — **Main entry point**

.. code-block:: cpp

   int main(int argc, char** argv) {
     return test<BasicTests>(argc, argv);
   }

``test<T>`` discovers all ``Test``-annotated methods via reflection, runs them, and prints results.
