Writing Your First Tests
========================

In this tutorial, we will create a simple test suite using the testing framework. We will create an ``subtract`` function and write tests to verify its correctness.

To get started, first make sure you have installed the library as described in the :ref:`installation guide<installation>`. Next, create a folder to use for this project, this is where we will put all of our files.

The first task is to create the `main.cpp` file in which we will put our function and test cases. You should initially put this `subtract` function in the file:

.. code-block:: cpp

    unsigned int subtract(unsigned int a, unsigned int b) pre(a > b) {
        return a - b;
    }

Note, the placement of the contract clause to ensure to act as a verification of proper parameters. We will test this contract as well in our test cases.

To write your first test, put the following block of code at the top of the file:

.. code-block:: cpp

    #include <annotest_contract.h>

    import annotest;

    using namespace annotest;

This imports the testing framework and allows us to use its features. Importantly, notice that it is an ``import`` statement and not a ``#include`` directive which improves compile times and avoids macro leakage. The include at the top is something we explain in :ref:`contract handling example<contract_handling>`. We also use the namespace for simplicity.

Next, we will define a struct to hold our test cases and group them.

.. code-block:: cpp

    struct SubtractTests {
        ...
    };

Now to write our first test case, we will create a regular method and place the ``[[=Test{}]]`` annotation before it. This tells the framework that this method is a test case and should be run as a test. The function name will be used as our display name for the test case. For more properties of the ``Test`` annotation, see the :ref:`API documentation <api_annotations>`. Let's write our first test case to verify that the subtraction works correctly:

.. code-block:: cpp

    struct SubtractTests {
        [[=Test{}]] void subtraction_works() {
            assertEqual(5, subtract(10, 5));
        }
    };

Now, one last thing to do in this file. We need to add a ``main()`` function that actually calls the library to run our tests. To do this, create this main function at the bottom of the file:

.. code-block:: cpp

    int main(int argc, char** argv) {
        return test<SubtractTests>(argc, argv);
    }

The ``test<T>`` function is the entry point for the library tests. It discovers all tests in the ``SubtractTests`` suite and runs them. We pass ``argc`` and ``argv`` to give the library access to command-line arguments for usage with CTest.

That's it. That is your first test suite. Now to build it, create a ``CMakeLists.txt`` file in the same folder with the following content:

.. code-block:: cmake

    cmake_minimum_required(VERSION 4.3)

    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b") # Enable the std module for the library
    set(CMAKE_CXX_MODULE_STD 1)

    project(SubtractTests LANGUAGES CXX)

    include(CTest)

    find_package(AnnoTest REQUIRED) # Discover the library

    add_executable(subtract_tests main.cpp)
    target_link_libraries(subtract_tests PRIVATE annotest)

    enable_testing()
    annotest_discover_tests(subtract_tests) # Register tests with CTest

As you can see, the library provides a custom CMake function, ``annotest_discover_tests()``, that registers the tests with CTest.

Now, run ``cmake -B build -S . && cmake --build build`` to build the tests. Finally, to run the test, execute ``ctest --test-dir build/``. You should see output indicating that the test passed. Here is example output:

.. code-block:: console

    Test project /path/to/your/folder/
        Start 1: SubtractTests.subtraction_works
    1/1 Test #1: SubtractTests.subtraction_works ...   Passed    0.00 sec

    100% tests passed, 0 tests failed out of 1

    Total Test time (real) =   0.00 sec

Congratulations! You have successfully written and run your first test using the library. But we have one more test that we can write. Our contract. We need to verify that it works. So, let's add another test case to verify that the contract is properly enforced. Add the following test case to the ``SubtractTests`` struct:

.. code-block:: cpp

    // rest of struct
    [[=Test{}]] void subtraction_contract_works() {
        assertContractViolation([]() { subtract(5, 10); });
    }
    // rest of struct

This test case uses the ``assertContractViolation()`` assertion to verify that the contract is violated when we try to subtract a larger number from a smaller number. Now, rebuild and run the tests again. You should see that both tests pass:

.. code-block:: console

    Test project /path/to/your/folder/
        Start 1: SubtractTests.subtraction_works
    1/2 Test #1: SubtractTests.subtraction_works ............   Passed    0.00 sec
        Start 2: SubtractTests.subtraction_contract_works
    2/2 Test #2: SubtractTests.subtraction_contract_works ...   Passed    0.00 sec

    100% tests passed, 0 tests failed out of 2

    Total Test time (real) =   0.01 sec

Amazing! You have officially done your first tests with the library. For a full list of assertions and expectations, checkout API documentation :ref:`here<assertion>` and :ref:`here<expectation>`. You can find the full source code `here <https://www.github.com/EJainDev/AnnoTest/tree/main/examples/basic_testing>`__. **IMPORTANT**: If you build examples from the library GitHub repo, your ``ctest`` testing directory changes to ``build/examples/release/your_current_example_folder``.