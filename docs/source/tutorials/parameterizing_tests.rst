Parameterizing Your Tests
=========================

In the previous tutorial, we wrote some basic tests. In this tutorial, we will learn how to parameterize our tests to run the same test logic with different inputs. This is useful for testing a function with multiple sets of data without having to write separate test cases for each set.

Why Parameterize?
-----------------

Take a look at the code we wrote last time for the subtraction tests. Our two test cases each covered one specific set of arguments to the function:

.. code-block:: cpp

    struct SubtractTests {
        [[=Test{}]] void subtraction_works() {
            assertEqual(5, subtract(10, 5));
        }

        [[=Test{}]] void subtraction_contract_works() {
            assertContractViolation([]() { subtract(5, 10); });
        }
    };

This is fine, but what if we want to test more cases? Then, we could test more cases with more tests, right?

.. code-block:: cpp

    struct SubtractTests {
        [[=Test{}]] void subtraction_works() {
            assertEqual(5, subtract(10, 5));
        }

        [[=Test{}]] void subtraction_with_zero() {
            assertEqual(10, subtract(10, 0));
        }

        [[=Test{}]] void subtraction_with_equal_numbers() {
            assertEqual(0, subtract(10, 10));
        }

        [[=Test{}]] void subtraction_with_large_numbers() {
            assertEqual(1000000, subtract(1000005, 5));
        }
    };

Or, we could simply merge them:

.. code-block:: cpp

    struct SubtractTests {
        [[=Test{}]] void subtraction_works() {
            assertEqual(5, subtract(10, 5));
            assertEqual(10, subtract(10, 0));
            assertEqual(0, subtract(10, 10));
            assertEqual(1000000, subtract(1000005, 5));
        }

        [[=Test{}]] void subtraction_contract_works() {
            assertContractViolation([]() { subtract(5, 10); });
        }
    };

But there are cons to each approach here. Scenario one has too much code duplication and scenario two causes us to debug to find out which assertion failed. And, it also has some code duplication. The solution is to parameterize our tests.

Using the Parameterize Test Annotation
--------------------------------------

To parameterize our tests, we can use the ``Parameterize`` test annotation. This allows us to define a test that will run multiple times with different parameters. Here's how we can rewrite our subtraction tests using parameterization:

.. code-block:: cpp

    struct SubtractTests {
        [[=Test{}, =Parameterize{tuple(10u, 5u), tuple(10u, 0u), tuple(10u, 10u), tuple(1000005u, 5u)}]] void subtraction_works(unsigned int a, unsigned int b) {
            const auto expected =a - b;
            assertEqual(expected, subtract(a, b));
        }

        [[=Test{}]] void subtraction_contract_works() {
            assertContractViolation([]() { subtract(5, 10); });
        }
    };

Look at that, no code duplication, one test, and all the scenarios we want. An important distinction to make here is the use of ``tuple``. This IS NOT ``std::tuple``. It is a helper function exposed by the ``annotest::`` namespace for this library ONLY.

And that's all you have to change. Now, you can rebuild and run your tests. Here is the expected output:

.. code-block:: console

    Running parameterized test subtraction_works with 4 parameter sets
        - {10   - {5} --  passed in 0.000512 ms
        - {10   - {0} --  passed in 0.000148 ms
        - {10   - {10} --  failed with error: Contract violation occurred during test execution
        - {1000005      - {5} --  passed in 0.000141 ms
    Running test: subtraction_contract_works
    Passed in 0.000513 ms

Wait what? Our test failed? Yes, it did and that is the importance of testing. If you take a closer look at our subtraction function, it fails even if the result would be 0 which is incorrect. If you fix that by using `>=` instead of `>` and run it again:

.. code-block:: console
    Running parameterized test subtraction_works with 4 parameter sets
        - {10   - {5} --  passed in 0.00036 ms
        - {10   - {0} --  passed in 0.000127 ms
        - {10   - {10} --  passed in 0.000104 ms
        - {1000005      - {5} --  passed in 6.4e-05 ms
    Running test: subtraction_contract_works
    Passed in 0.000331 ms

This should match your output now. All tests should pass and you have successfully parameterized your tests! Now, you can easily add more test cases by simply adding more tuples to the ``Parameterize`` annotation.