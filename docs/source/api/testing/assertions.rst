.. _assertions_and_expectations:

Assertions and expectations
===========================

.. cpp:namespace:: annotest

A successful check returns normally. A failed ``assert_*`` records a fatal failure
and unwinds its callable; a failed ``expect_*`` records a nonfatal failure and
returns. Both fail the case. Arguments are evaluated once; values are formatted
on failure. Unless an entry states otherwise, checks return ``void``.

The ``loc`` or ``location`` argument defaults to the calling source location.
Message overloads add user context without changing the comparison.
The basic overloads take their operands by value except where shown.

Equality and ordering
---------------------

.. cpp:function:: void assert_equal(auto expected, auto actual, const std::source_location& loc = std::source_location::current())

   Succeeds when ``expected != actual`` is false. C-string operands compare pointer identity; use ``assert_streq`` for content.

   :param expected: Expected value; must support comparison with actual.
   :param actual: Observed value.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_equal(auto expected, auto actual, const std::source_location& loc = std::source_location::current())

   Succeeds when ``expected != actual`` is false. C-string operands compare pointer identity; use ``assert_streq`` for content.

   :param expected: Expected value; must support comparison with actual.
   :param actual: Observed value.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_not_equal(auto expected, auto actual, const std::source_location& loc = std::source_location::current())

   Succeeds when ``expected == actual`` is false.

   :param expected: Value that actual must differ from.
   :param actual: Observed value.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_not_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_not_equal(auto expected, auto actual, const std::source_location& loc = std::source_location::current())

   Succeeds when ``expected == actual`` is false.

   :param expected: Value that actual must differ from.
   :param actual: Observed value.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_not_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_less(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a < b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_less(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_less(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a < b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_less(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_less_equal(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a <= b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_less_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_less_equal(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a <= b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_less_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_greater(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a > b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_greater(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_greater(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a > b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_greater(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_greater_equal(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a >= b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_greater_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_greater_equal(auto a, auto b, const std::source_location& loc = std::source_location::current())

   Succeeds when ``a >= b``.

   :param a: Left comparison operand.
   :param b: Right comparison operand.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_greater_equal(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

Truth, null, and containment
----------------------------

.. cpp:function:: void assert_true(auto value, const std::source_location& loc = std::source_location::current())

   Succeeds when the value tests true in a Boolean context.

   :param value: Expression supporting Boolean testing.
   :returns: Nothing.

.. cpp:function:: template <typename T> void assert_true(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_true(auto value, const std::source_location& loc = std::source_location::current())

   Succeeds when the value tests true in a Boolean context.

   :param value: Expression supporting Boolean testing.
   :returns: Nothing.

.. cpp:function:: template <typename T> void expect_true(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_false(auto value, const std::source_location& loc = std::source_location::current())

   Succeeds when the value tests false in a Boolean context.

   :param value: Expression supporting Boolean testing.
   :returns: Nothing.

.. cpp:function:: template <typename T> void assert_false(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_false(auto value, const std::source_location& loc = std::source_location::current())

   Succeeds when the value tests false in a Boolean context.

   :param value: Expression supporting Boolean testing.
   :returns: Nothing.

.. cpp:function:: template <typename T> void expect_false(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_null(auto ptr, const std::source_location& loc = std::source_location::current())

   Succeeds when the pointer compares equal to ``nullptr``.

   :param ptr: Nullable pointer.
   :returns: Nothing.

.. cpp:function:: template <typename T> void assert_null(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_null(auto ptr, const std::source_location& loc = std::source_location::current())

   Succeeds when the pointer compares equal to ``nullptr``.

   :param ptr: Nullable pointer.
   :returns: Nothing.

.. cpp:function:: template <typename T> void expect_null(T&& value, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void assert_contains(auto const& container, auto const& value, const std::source_location& loc = std::source_location::current())

   Succeeds when ``std::find`` finds an equal element before the end of the container.

   :param container: Iterable container.
   :param value: Element to find.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_contains(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_contains(auto const& container, auto const& value, const std::source_location& loc = std::source_location::current())

   Succeeds when ``std::find`` finds an equal element before the end of the container.

   :param container: Iterable container.
   :param value: Element to find.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_contains(T&& first, U&& second, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

Numeric tolerance
-----------------

.. cpp:function:: template <typename T, typename U, typename K = std::common_type_t<T, U>> void assert_near(T expected, U actual, K tol = static_cast<K>(0.001), const std::source_location& loc = std::source_location::current())

   Succeeds when ``std::abs(expected - actual) <= tol``. The default tolerance is ``static_cast<K>(0.001)``; integral common types therefore default to zero.

   :param expected: Expected numeric value.
   :param actual: Observed numeric value.
   :param tol: Inclusive absolute tolerance; K defaults to the operands' common type.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U, typename K> void assert_near(T expected, U actual, K tolerance, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: template <typename T, typename U, typename K = std::common_type_t<T, U>> void expect_near(T expected, U actual, K tol = static_cast<K>(0.001), const std::source_location& loc = std::source_location::current())

   Succeeds when ``std::abs(expected - actual) <= tol``. The default tolerance is ``static_cast<K>(0.001)``; integral common types therefore default to zero.

   :param expected: Expected numeric value.
   :param actual: Observed numeric value.
   :param tol: Inclusive absolute tolerance; K defaults to the operands' common type.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U, typename K> void expect_near(T expected, U actual, K tolerance, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

C-string content
----------------

.. cpp:function:: template <typename T, typename U> void assert_streq(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when null-terminated strings have equal contents. Two null pointers are equal; null differs from an empty string.

   :param expected: Narrow or wide C string; both operands must use the same character type.
   :param actual: Observed C string.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_streq(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when null-terminated strings have equal contents. Two null pointers are equal; null differs from an empty string.

   :param expected: Narrow or wide C string; both operands must use the same character type.
   :param actual: Observed C string.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_strne(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when C-string contents differ, including null versus non-null.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_strne(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when C-string contents differ, including null versus non-null.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_strcaseeq(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when contents match after ASCII case folding. Unicode case folding is not performed.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_strcaseeq(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when contents match after ASCII case folding. Unicode case folding is not performed.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void assert_strcasene(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when contents differ after ASCII case folding.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename U> void expect_strcasene(T expected, U actual, std::string_view message = {}, std::source_location location = std::source_location::current())

   Succeeds when contents differ after ASCII case folding.

   :param expected: Narrow or wide C string.
   :param actual: C string of the same character type.
   :param message: Optional diagnostic context.
   :returns: Nothing.

Exceptions
----------

.. cpp:function:: template <typename E = std::exception> void assert_throws(auto func, const std::source_location& loc = std::source_location::current())

   Invokes the callable once. Succeeds if an exception caught by ``const E&`` is thrown, including derived types. No exception or a different type fails. A library skip propagates.

   :param func: Nullary callable.
   :param E: Expected exception type; defaults to std::exception.
   :returns: Nothing.

.. cpp:function:: template <typename E = std::exception, typename F> void assert_throws(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: template <typename E = std::exception> void expect_throws(auto func, const std::source_location& loc = std::source_location::current())

   Invokes the callable once. Succeeds if an exception caught by ``const E&`` is thrown, including derived types. No exception or a different type fails. A library skip propagates.

   :param func: Nullary callable.
   :param E: Expected exception type; defaults to std::exception.
   :returns: Nothing.

.. cpp:function:: template <typename E = std::exception, typename F> void expect_throws(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: template <typename E> void assert_throws_exact(auto f, const std::source_location& loc = std::source_location::current())

   Invokes the callable once. Succeeds only if the dynamic exception type is exactly E. A derived type, another exception, or no exception fails. A library skip propagates.

   :param f: Nullary callable.
   :param E: Exact exception type; specify E explicitly for the overload without a message.
   :returns: Nothing.

.. cpp:function:: template <typename E = std::exception, typename F> void assert_throws_exact(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: template <typename E> void expect_throws_exact(auto f, const std::source_location& loc = std::source_location::current())

   Invokes the callable once. Succeeds only if the dynamic exception type is exactly E. A derived type, another exception, or no exception fails. A library skip propagates.

   :param f: Nullary callable.
   :param E: Exact exception type; specify E explicitly for the overload without a message.
   :returns: Nothing.

.. cpp:function:: template <typename E = std::exception, typename F> void expect_throws_exact(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

Failure inspection
------------------

.. cpp:function:: bool has_failure()

   Return whether the active case has any recorded failure.

.. cpp:function:: bool has_fatal_failure()

   Return whether the active case has a recorded fatal assertion failure.

.. cpp:function:: bool has_nonfatal_failure()

   Return whether the active case has a recorded expectation failure.

.. cpp:function:: std::vector<failure_result> current_failures()

   Return a snapshot of recorded failures for the active case. Each entry contains a category, message, phase, and optional source location; see :doc:`results`.

.. cpp:function:: template <typename F> std::vector<failure_result> capture_failures(F&& callable, bool all_threads = false)

   Invoke the callable and return intercepted failures without failing the surrounding case. A fatal assertion ends the callable. Nested captures are independent; exceptions other than assertion control flow propagate.

   :param callable: Nullary callable to inspect.
   :param all_threads: Also intercept helper-thread failures when true. Join all such threads before the callable returns.
   :returns: Captured failure records.
   :throws std::logic_error: Another all-thread capture is already active.

   Intercepted failures do not appear in current_failures() or its Boolean query helpers.

.. cpp:function:: template <typename F> void assert_failure(F&& callable, failure_types type, std::string_view substring = {}, std::source_location location = std::source_location::current())

   Capture failures from a callable and require a failure of the selected type whose message contains the supplied substring.

   :param callable: Nullary callable.
   :param type: Required failure category; see :cpp:enum:`failure_types`.
   :param substring: Required message fragment; an empty string accepts any message.
   :returns: Nothing.

.. cpp:function:: template <typename F> void expect_failure(F&& callable, failure_types type, std::string_view substring = {}, std::source_location location = std::source_location::current())

   Capture failures from a callable and require a failure of the selected type whose message contains the supplied substring.

   :param callable: Nullary callable.
   :param type: Required failure category; see :cpp:enum:`failure_types`.
   :param substring: Required message fragment; an empty string accepts any message.
   :returns: Nothing.

.. cpp:function:: template <typename F> void assert_no_fatal_failure(F&& callable, std::source_location location = std::source_location::current())

   Run a callable and fail this check if it records a fatal assertion. Nonfatal failures remain recorded.

   :param callable: Nullary callable.
   :returns: Nothing.

.. cpp:function:: template <typename F> void expect_no_fatal_failure(F&& callable, std::source_location location = std::source_location::current())

   Run a callable and fail this check if it records a fatal assertion. Nonfatal failures remain recorded.

   :param callable: Nullary callable.
   :returns: Nothing.

Explicit failures and exception guards
--------------------------------------

.. cpp:function:: void fail(std::string_view message = "Explicit failure", std::source_location location = std::source_location::current())

   Record a fatal failure and unwind the callable.

   :param message: Diagnostic text; defaults to ``Explicit failure``.
   :returns: Does not return normally.

.. cpp:function:: void add_failure_message(std::string_view message, std::source_location location = std::source_location::current())

   Record a nonfatal expectation failure and continue.

   :param message: Diagnostic text.
   :returns: Nothing.

.. cpp:function:: void assert_no_throw(auto callable, std::source_location location = std::source_location::current())

   Invoke a callable once and require it to complete without an exception. A library skip propagates.

   :param callable: Nullary callable.
   :returns: Nothing.

.. cpp:function:: template <typename F> void assert_no_throw(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

.. cpp:function:: void expect_no_throw(auto callable, std::source_location location = std::source_location::current())

   Invoke a callable once and require it to complete without an exception. A library skip propagates.

   :param callable: Nullary callable.
   :returns: Nothing.

.. cpp:function:: template <typename F> void expect_no_throw(F&& callable, std::string_view message, std::source_location location = std::source_location::current())

   Message overload. The check has the same operands and behavior as the overload above.

   :param message: User context appended to the failure diagnostic.
   :param location: Source location, defaulted to the call site.

Assertion groups
----------------

.. cpp:function:: template <typename... Fs> void assert_all(std::string_view name, Fs&&... blocks)

   Run callable blocks in order. A fatal assertion ends its block; later blocks still run. Standard and unknown exceptions become errors. A skip stops the group. Recorded failures retain their original locations and categories.

   :param blocks: Zero or more nullary callables; move-only callables are accepted.
   :param name: Group label added as trace context.
   :returns: Nothing. Unwinds after all blocks if a new failure was recorded.

.. cpp:function:: template <typename... Fs> void assert_all(Fs&&... blocks)

   Run callable blocks in order. A fatal assertion ends its block; later blocks still run. Standard and unknown exceptions become errors. A skip stops the group. Recorded failures retain their original locations and categories.

   :param blocks: Zero or more nullary callables; move-only callables are accepted.
   :returns: Nothing. Unwinds after all blocks if a new failure was recorded.

.. cpp:function:: template <typename... Fs> void expect_all(std::string_view name, Fs&&... blocks)

   Run callable blocks in order. A fatal assertion ends its block; later blocks still run. Standard and unknown exceptions become errors. A skip stops the group. Recorded failures retain their original locations and categories.

   :param blocks: Zero or more nullary callables; move-only callables are accepted.
   :param name: Group label added as trace context.
   :returns: Nothing. Continues after the group even if blocks failed.

.. cpp:function:: template <typename... Fs> void expect_all(Fs&&... blocks)

   Run callable blocks in order. A fatal assertion ends its block; later blocks still run. Standard and unknown exceptions become errors. A skip stops the group. Recorded failures retain their original locations and categories.

   :param blocks: Zero or more nullary callables; move-only callables are accepted.
   :returns: Nothing. Continues after the group even if blocks failed.

Deferred calls and diagnostic context
-------------------------------------

.. cpp:function:: auto wrap(auto f, auto&&... args)

   Bind a callable and arguments into a nullary callable. The callable and decayed argument tuple are stored by value; use ``std::ref`` for referenced arguments.

   :param f: Callable invoked with the stored arguments.
   :param args: Arguments forwarded into tuple storage.
   :returns: Mutable callable returning the bound invocation's result.

.. cpp:class:: scoped_trace

   Add thread-local diagnostic context until destruction. Traces are included in failures on the creating thread. The type is noncopyable.

.. cpp:function:: explicit scoped_trace::scoped_trace(std::string message, std::source_location location = std::source_location::current())

   Push a message and its source location. Destruction removes it.

   :param message: Context to attach to subsequent failures.
   :param location: Location of the trace declaration.

.. cpp:type:: scoped_message = scoped_trace

   Alias for :cpp:class:`scoped_trace`.

Diagnostic printers
-------------------

Define ``void print_to(const T&, std::ostream*)`` in the value's namespace
(or as a friend). Argument-dependent lookup selects it before standard
formatting, range printing, or stream insertion. It also applies inside ranges.
Printers run only on a failing check. Invalid wide characters are escaped and
valid wide strings are encoded as UTF-8.

For matcher-based checks, see :doc:`matchers`. Contract and process checks are
specified in :doc:`contracts` and :doc:`death`.
