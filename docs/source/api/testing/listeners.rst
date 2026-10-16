Listeners and extensions
========================

.. cpp:namespace:: annotest

Register listeners before calling a runner. Callback arguments are borrowed
for the duration of the callback; copy data needed later.

.. cpp:struct:: test_listener

   Polymorphic observer and extension interface. Every callback has a default no-op implementation; the destructor is virtual.

.. cpp:function:: virtual void test_listener::on_discovery(std::string_view suite, std::string_view name)

   Receive the suite and case names during discovery.

.. cpp:function:: virtual void test_listener::on_test_discovered(const test_info& info)

   Receive full discovered-case metadata, including disabled status, tags, parameters, and source location.

.. cpp:function:: virtual void test_listener::on_suite_start(std::string_view suite)

   Observe the start of a selected suite.

.. cpp:function:: virtual void test_listener::on_suite_end(std::string_view suite)

   Observe the end of a selected suite.

.. cpp:function:: virtual void test_listener::on_test_start(std::string_view suite, std::string_view name)

   Observe a case starting.

.. cpp:function:: virtual void test_listener::on_assertion(const assertion_event& event)

   Receive a recorded failure's message, phase, and optional source location. Callback invocations are serialized.

.. cpp:function:: virtual void test_listener::on_test_end(const test_result& result)

   Receive a completed case result.

.. cpp:function:: virtual void test_listener::on_suite_result(const suite_result& result)

   Receive final suite counts and duration, including fixture teardown failures.

.. cpp:function:: virtual void test_listener::on_run_end(std::span<const test_result> results)

   Receive completed results within this listener's scope. Implement custom reporting here.

.. cpp:function:: virtual void test_listener::before_test()

   Extension setup before fixture construction. Runs in registration order.

.. cpp:function:: virtual void test_listener::after_test()

   Extension cleanup after fixture teardown. Runs in reverse order for each listener whose setup was entered, even when that setup threw.

.. cpp:function:: void add_listener(std::shared_ptr<test_listener> listener, std::string suite_scope = {})

   Retain shared ownership of an observer/extension.

   :param listener: Non-null listener instance.
   :param suite_scope: Exact suite name and its nested suites; empty selects every suite.
   :returns: Nothing.
   :throws std::invalid_argument: listener is null.

.. cpp:function:: void clear_listeners()

   Remove all listener registrations and scopes.

Observer exceptions become separate failure entries and do not discard
completed results or the XML report. In isolated execution, extension hooks
run in workers; assertion events and result callbacks run in the parent.
Result data structures are specified in :doc:`results`.
