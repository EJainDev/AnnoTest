Runner and registration
=======================

.. cpp:namespace:: annotest

.. cpp:function:: template <typename... Ts> int run_tests(int argc, char** argv)

   Discover annotated suites, combine them with runtime registrations, execute the selection, and write requested reports. Ts must contain one or more class types.

   :param argc: Argument count from main.
   :param argv: Argument vector from main; forward unchanged for subprocess replay.
   :returns: 0 on success, listing, or help; 1 on test failure, invalid arguments, discovery errors, or report failure.

   Execution and output capture are process-wide. Recursive or simultaneous in-process runners are rejected. Use workers for parallel execution. Option syntax is in :doc:`command_line`.

.. cpp:function:: int run_registered_tests(int argc, char** argv)

   Run runtime registrations without discovering annotated suite types.

   :param argc: Argument count from main.
   :param argv: Argument vector from main.
   :returns: Same exit status as run_tests.

Runtime registration
--------------------

.. cpp:function:: template <typename Body> void register_test(std::string suite, std::string name, Body body, std::vector<std::string> tags = {}, std::source_location location = std::source_location::current())

   Register a nullary callable before execution. Body must be invocable as Body&; move-only callables are retained through shared ownership.

   :param suite: Suite display name.
   :param name: Case display name.
   :param body: Case callable.
   :param tags: Optional selection labels.
   :param location: Registration source location.
   :returns: Nothing.

   Names must be nonempty with no tabs or line breaks. Repeat registration in each worker invocation when using isolation.

.. cpp:function:: template <typename Fixture, typename Factory, typename Body> void register_test(std::string suite, std::string name, Factory factory, Body body, std::vector<std::string> tags = {}, std::source_location location = std::source_location::current())

   Register a fixture-backed case with annotated lifecycle hooks.

   :param suite: Suite display name.
   :param name: Case display name.
   :param factory: Callable returning std::unique_ptr<Fixture>, accepting no arguments or test_context&.
   :param body: Callable accepting Fixture&.
   :param tags: Optional selection labels.
   :param location: Registration source location.
   :returns: Nothing.

   Registrations of the same fixture type and suite name share suite hooks, including annotated cases of that type in the same suite.

.. cpp:function:: void clear_registered_tests()

   Remove all runtime registrations. Call before a later runner invocation to discard retained callables.

Skipping
--------

.. cpp:function:: [[noreturn]] inline void skip(std::string reason = "Assumption not met")

   Abort the current body with a skip reason. Cleanup still runs; cleanup failure overrides the skip.

   :param reason: Reported skip reason.
   :returns: Does not return normally.

.. cpp:function:: void assume(bool condition, std::string reason = "Assumption not met")

   Continue if the condition is true; otherwise call skip(reason).

   :param condition: Required runtime prerequisite.
   :param reason: Reported skip reason.
   :returns: Nothing when the condition is true.

Helper threads
--------------

.. cpp:function:: std::jthread test_thread(std::function<void()> body)

   Create a jthread wrapping a test callable. Fatal assertions unwind that callable; unexpected exceptions become active-case failures. Nonfatal expectations continue.

   :param body: Nullary callable.
   :returns: Owning std::jthread; its destructor joins.

   Join every helper thread before the test returns. Failures attach to the active case in arrival order. Traces remain thread-local and assertion callbacks are serialized.

Fixtures, nested suites, lifecycle hooks, repetitions, and runtime conditions
are specified in :doc:`annotations`. For their execution order and process
boundaries, see :doc:`/explanation/execution`.
