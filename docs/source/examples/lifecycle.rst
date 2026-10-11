Lifecycle Hooks
===============

Raw Source
----------

.. literalinclude:: ../../../examples/example02_lifecycle.cpp
   :language: cpp

Breaking It Down
----------------

Section 1 — **before_all**

.. code-block:: cpp

   [[= before_all{}]] void setUp() {
     call_count = 0;
     std::cout << "[before_all] Setup shared resources\n";
   }

Runs **once** before the first test. If it throws, the entire suite aborts.

Section 2 — **before_each**

.. code-block:: cpp

   [[= before_each{}]] void resetCounters() { call_count = 0; }

Runs before **every** test. Resets per-test fixture state.

Section 3 — **after_each**

.. code-block:: cpp

   [[= after_each{}]] void reportCounts() {
     std::cout << "[after_each] counter = " << call_count << "\n";
   }

Runs after **every** test. If it throws, only a warning is printed (suite continues).

Section 4 — **after_all**

.. code-block:: cpp

   [[= after_all{}]] void tearDown() {
     std::cout << "[after_all] Cleaning up shared resources\n";
   }

Runs **once** after the last test. If it throws, only a warning is printed.

Section 5 — **Tests use the hook state**

.. code-block:: cpp

   [[= Test{}]] void test_one() {
     ++call_count;
     assert_equal(1, call_count);
   }

Each test runs with a fresh reset by ``before_each``, so the counter is always 1 at this point.

Execution order:

.. code-block:: text

   before_all()
     before_each() → test_one() → after_each()
     before_each() → test_two() → after_each()
     before_each() → test_three() → after_each()
   after_all()
