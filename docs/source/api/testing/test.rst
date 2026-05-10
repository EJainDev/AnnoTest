Testing Annotations and Functions
=================================

.. cpp:namespace:: cpputils::testing

Enums
-----

.. cpp:enum-class:: OS

    Represents the different operating systems to enable/disable tests on specific platforms.

    .. cpp:enumerator:: Windows

        Represents the Windows operating system.

    .. cpp:enumerator:: Linux

        Represents the Linux operating system.

    .. cpp:enumerator:: Mac

        Represents the macOS operating system.

    .. cpp:enumerator:: Unknown

        Represents an unknown operating system.

Annotations
-----------

.. cpp:struct:: BeforeAll

    A struct that annotates a function to be executed *once* before all tests in a test suite.

    .. note:: If this function throws an exception, the test suite is aborted

    .. note:: Running with CTest will cause this function to be executed before each test case which may significantly slow down your tests!

.. cpp:struct:: BeforeEach

    A struct that annotates a function to be executed before *each* test in a test suite.

    .. note:: If this function throws an exception, the test is aborted

    .. important:: Only one function can be annotated with `BeforeEach` in a test suite. If multiple functions are annotated, only the last one is executed.

.. cpp:struct:: Test

    A struct that annotates a function as a test case.

    .. important:: A test must not use try-catch blocks around any assert or expect calls for them to work! Use the expect/assert functions for these instead.

.. cpp:struct:: AfterEach

    A struct that annotates a function to be executed after *each* test in a test suite.

    .. note:: If this function throws an exception, only a warning is printed

    .. important:: Only one function can be annotated with `AfterEach` in a test suite. If multiple functions are annotated, only the last one is executed.

.. cpp:struct:: AfterAll

    A struct that annotates a function to be executed *once* after all tests in a test suite.

    .. note:: If this function throws an exception, only a warning is printed

Utility Annotations
^^^^^^^^^^^^^^^^^^^

.. cpp:struct:: Disabled

    A struct that annotates a test case to be disabled causing the library to print that it is disabled and skip the test.

.. cpp:struct:: Parameterize

    A struct that annotates a test case to be parameterized with different sets of parameters. Each set of parameters is tested as a "sub" test case. To use, pass in any amount of tuples created using :cpp:func:`~tuple` with each tuple representing a **complete** set of parameters for that function, i.e., each value corresponds to a parameter of the function in order.

.. cpp:struct:: template <int N> RequiresOS

    A struct that annotates a test case to only run on specific operating systems by passing in N :cpp:enum:`~OS` values. If the test is run on an unsupported operating system, the library will print that it is unsupported and skip the test.

    .. cpp:member:: OS os[N]

        The array of supported operating systems (may be of length 0).

.. cpp:struct:: template <int N> DisallowsOS

    A struct that annotes a test case to not run on specific operating systems by passing in N :cpp:enum:`~OS` values. If the test is run on a disallowed operating system, the library will print that it is unsupported and skip the test. Oppositie of :cpp:struct:`~RequiresOS`.

    .. cpp:member:: OS os[N]

        The array of disallowed operating systems (may be of length 0).

Functions
---------

.. cpp:function:: template <std::is_class T> int test(int argc, char** argv, T suite = {})

    The main function of the library that runs the tests in a test suite and prints the results handling all of the above annotations.

    This function takes in the command line arguments for CTest specific features such as running one test a time or listing tests.

    Returns 0 if all tests passed or were aborted and 1 if any test failed. Note that aborted tests do not cause the function to return 1 since they are not considered failed.