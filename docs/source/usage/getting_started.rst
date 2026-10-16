.. _getting_started:

Write and run your first test
=============================

In this tutorial, create a small test executable, run it, and inspect a deliberate
failure. By the end, the executable will also expose its case through CTest.

Before starting
---------------

Use the toolchain from :doc:`/installation`: GCC 16 or newer with C++26 reflection,
CMake 4.3.3 or newer, and Ninja. The commands below start in an AnnoTest source
checkout. Git and network access are needed if dependency packages are not
already available. On Windows, use UCRT64 and append ``.exe`` when running binaries.

Create a project directory inside the checkout:

.. code-block:: sh

   mkdir -p build/first-test

Write one check
---------------

Save the following as ``build/first-test/main.cpp``:

.. code-block:: cpp
   :caption: main.cpp

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

The annotation marks ``addition`` as a test method. The assertion compares the
expected value, 4, with the actual expression, ``2 + 2``. The runner receives the
command line so it can list and select cases.

Connect the library
-------------------

Save this as ``build/first-test/CMakeLists.txt``:

.. code-block:: cmake
   :caption: CMakeLists.txt

   cmake_minimum_required(VERSION 4.3.3)
   set(CMAKE_CXX_STANDARD 26)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   if(CMAKE_VERSION VERSION_LESS 4.4)
     set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
   else()
     set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
   endif()
   set(CMAKE_CXX_MODULE_STD ON)
   project(FirstTest LANGUAGES CXX)
   if(CMAKE_VERSION VERSION_LESS 4.4)
     string(APPEND CMAKE_CXX_FLAGS " -freflection")
   endif()

   set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
   set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
   add_subdirectory(../.. annotest)
   enable_testing()
   add_executable(first_test main.cpp)
   target_link_libraries(first_test PRIVATE annotest::annotest)
   annotest_discover_tests(first_test)

The two-directory relative path points back to the AnnoTest checkout.
The module settings appear before ``project()`` so CMake can configure the
standard-library module. The discovery helper registers the executable's cases
with CTest after building it.

Build and run
-------------

From the checkout root, configure and build:

.. code-block:: sh

   cmake -S build/first-test -B build/first-test/build -G Ninja \
     -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug
   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test

The run reports one passing case and returns exit status 0. List the case
without running its body:

.. code-block:: sh

   ./build/first-test/build/first_test --list

The listing identifies ``ArithmeticTests.addition``. Run that same case through
CTest:

.. code-block:: sh

   ctest --test-dir build/first-test/build --output-on-failure

CTest reports one test passed.

Inspect a failure
-----------------

Change ``assert_equal(4, 2 + 2)`` to ``assert_equal(5, 2 + 2)`` in ``main.cpp``.
Rebuild and select the case explicitly:

.. code-block:: sh

   cmake --build build/first-test/build --parallel 2
   ./build/first-test/build/first_test --test-name ArithmeticTests.addition

This run returns exit status 1. Its diagnostic identifies the failed equality
and the source location: the expected value is 5, while the actual result is 4.
Restore the expected value to 4, rebuild, and run CTest again to finish with a
passing project.

Continue
--------

Use :doc:`/examples/lifecycle` to prepare fixture state before each case, or
:doc:`/examples/parameterized` to check several input sets.
For the distinction between fixture instances and processes, read
:doc:`/explanation/execution`.
