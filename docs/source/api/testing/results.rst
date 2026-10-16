Results and discovery records
=============================

.. cpp:namespace:: annotest

.. cpp:enum-class:: test_status

   Case outcome: passed, failed, or skipped.

.. cpp:enumerator:: test_status::passed

   Case completed successfully.

.. cpp:enumerator:: test_status::failed

   At least one failure was recorded.

.. cpp:enumerator:: test_status::skipped

   Case was skipped with a reason.

.. cpp:enum-class:: failure_types

   Recorded failure category.

.. cpp:enumerator:: failure_types::assertion

   Fatal assertion failure.

.. cpp:enumerator:: failure_types::expectation

   Nonfatal expectation failure.

.. cpp:enumerator:: failure_types::contract_violation

   Contract handler reported a violation.

.. cpp:enumerator:: failure_types::uncaught_exception

   Unexpected standard exception.

.. cpp:enumerator:: failure_types::unknown

   Failure without a more specific category.

failure_result
--------------

.. cpp:struct:: failure_result

   One recorded failure.

.. cpp:member:: failure_types failure_result::type

   Failure category.

.. cpp:member:: std::string failure_result::message

   Diagnostic text.

.. cpp:member:: std::string failure_result::phase

   Execution phase, such as setup, body, or cleanup.

.. cpp:member:: std::optional<std::pair<std::string, std::size_t>> failure_result::location

   File and line when available; empty otherwise.

assertion_event
---------------

.. cpp:struct:: assertion_event

   Borrowed callback record for a failure event.

.. cpp:member:: std::string assertion_event::message

   Diagnostic text.

.. cpp:member:: std::string assertion_event::phase

   Execution phase.

.. cpp:member:: std::optional<std::pair<std::string, std::size_t>> assertion_event::location

   Optional source file and line.

test_info
---------

.. cpp:struct:: test_info

   Metadata supplied during discovery.

.. cpp:member:: std::string test_info::suite

   Full suite name.

.. cpp:member:: std::string test_info::name

   Concrete case name.

.. cpp:member:: std::string test_info::file

   Source path.

.. cpp:member:: std::string test_info::parameters

   Formatted input description.

.. cpp:member:: std::size_t test_info::line = 0

   Source line; zero when unavailable.

.. cpp:member:: std::vector<std::string> test_info::tags

   Inherited and local labels.

.. cpp:member:: bool test_info::disabled = false

   Whether annotations disable the case.

test_result
-----------

.. cpp:struct:: test_result

   Completed case data supplied to reporters.

.. cpp:member:: std::string test_result::suite

   Full suite name.

.. cpp:member:: std::string test_result::name

   Concrete case name.

.. cpp:member:: test_status test_result::status = test_status::passed

   Final outcome.

.. cpp:member:: double test_result::seconds = 0

   Elapsed time in seconds.

.. cpp:member:: std::string test_result::skip_reason

   Reason when skipped.

.. cpp:member:: std::size_t test_result::failures = 0

   Recorded failure count.

.. cpp:member:: std::vector<failure_result> test_result::details

   Individual recorded failures.

.. cpp:member:: std::vector<std::pair<std::string, std::string>> test_result::properties

   Report properties, retaining duplicate names.

.. cpp:member:: std::string test_result::standard_output

   Captured standard output.

.. cpp:member:: std::string test_result::standard_error

   Captured standard error.

.. cpp:member:: std::string test_result::file

   Source path.

.. cpp:member:: std::string test_result::parameters

   Formatted input description.

.. cpp:member:: std::size_t test_result::line = 0

   Source line, or zero when unavailable.

.. cpp:member:: std::vector<std::string> test_result::tags

   Inherited and local labels.

suite_result
------------

.. cpp:struct:: suite_result

   Final aggregate counts and elapsed time.

.. cpp:member:: std::string suite_result::name

   Full suite name.

.. cpp:member:: unsigned suite_result::tests = 0

   Number of test results.

.. cpp:member:: unsigned suite_result::failed = 0

   Number of failed cases.

.. cpp:member:: unsigned suite_result::skipped = 0

   Number of skipped cases.

.. cpp:member:: unsigned suite_result::errors = 0

   Recorded error count.

.. cpp:member:: double suite_result::seconds = 0

   Elapsed seconds.

XML and JSON are described in :doc:`command_line`. The in-memory records keep
captured strings; report serialization renders malformed output bytes as visible
hexadecimal escapes. Callback lifetimes are specified in :doc:`listeners`.
