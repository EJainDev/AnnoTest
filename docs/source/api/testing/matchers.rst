Matchers
========

.. cpp:namespace:: annotest

Matchers are callable objects passed to :cpp:func:`assert_that` or
:cpp:func:`expect_that`. Factories below return a matcher by value and own their
arguments by value. The value under test is passed by const reference.
Requirements are checked when a matcher is applied to a value.

Applying matchers
-----------------

.. cpp:function:: template <typename T, typename M> void assert_that(const T& value, const M& match, std::string_view message = {}, std::source_location location = std::source_location::current())

   Apply a matcher once. A mismatch records a fatal assertion and unwinds the callable.

   :param value: Value under test.
   :param match: Callable accepting the value and returning an object with matched and explanation members, normally match_result. Wrap Boolean predicates with satisfies.
   :param message: Optional diagnostic context.
   :param location: Call-site source location.
   :returns: Nothing.

.. cpp:function:: template <typename T, typename M> void expect_that(const T& value, const M& match, std::string_view message = {}, std::source_location location = std::source_location::current())

   Apply a matcher once. A mismatch records a nonfatal expectation and returns.

   :param value: Value under test.
   :param match: Callable accepting the value and returning an object with matched and explanation members, normally match_result. Wrap Boolean predicates with satisfies.
   :param message: Optional diagnostic context.
   :param location: Call-site source location.
   :returns: Nothing.

Comparison
----------

.. cpp:function:: template <typename T> auto eq(T expected)

   Matches ``actual == expected``. C-string pointers use pointer identity; content checks are in :doc:`assertions`.

   :param expected: Owned expected value.
   :returns: A matcher object.

.. cpp:function:: template <typename T> auto ne(T value)

   Matches ``actual != value``.

   :param value: Owned comparison value.
   :returns: A matcher object.

.. cpp:function:: template <typename T> auto lt(T value)

   Matches ``actual < value``.

   :param value: Exclusive upper bound.
   :returns: A matcher object.

.. cpp:function:: template <typename T> auto le(T value)

   Matches ``actual <= value``.

   :param value: Inclusive upper bound.
   :returns: A matcher object.

.. cpp:function:: template <typename T> auto gt(T value)

   Matches ``actual > value``.

   :param value: Exclusive lower bound.
   :returns: A matcher object.

.. cpp:function:: template <typename T> auto ge(T value)

   Matches ``actual >= value``.

   :param value: Inclusive lower bound.
   :returns: A matcher object.

.. cpp:function:: auto anything()

   Matches every value.

   :returns: A matcher object.

.. cpp:function:: auto is_true()

   Matches values that convert to true.

   :returns: A matcher object.

.. cpp:function:: auto is_false()

   Matches values that convert to false.

   :returns: A matcher object.

.. cpp:function:: auto is_null()

   Matches values equal to nullptr.

   :returns: A matcher object.

.. cpp:function:: auto not_null()

   Matches values unequal to nullptr.

   :returns: A matcher object.

Composition
-----------

.. cpp:function:: template <typename M> auto negate(M inner)

   Matches when the inner matcher does not.

   :param inner: Matcher to invert.
   :returns: A matcher object.

.. cpp:function:: template <typename... Ms> auto all_of(Ms... inners)

   Matches when all inner matchers succeed, stopping at the first mismatch. With no arguments, always matches.

   :param inners: Zero or more matchers.
   :returns: A matcher object.

.. cpp:function:: template <typename... Ms> auto any_of(Ms... inners)

   Matches when any inner matcher succeeds, stopping at the first match. With no arguments, never matches.

   :param inners: Zero or more matchers.
   :returns: A matcher object.

Strings and regular expressions
-------------------------------

.. cpp:function:: auto starts_with(std::string prefix)

   Matches a narrow string or string view with the specified prefix; case-sensitive.

   :param prefix: Owned prefix string.
   :returns: A matcher object.

.. cpp:function:: auto ends_with(std::string suffix)

   Matches a narrow string or string view with the specified suffix; case-sensitive.

   :param suffix: Owned suffix string.
   :returns: A matcher object.

.. cpp:function:: auto has_substr(std::string text)

   Matches a narrow string or string view containing the specified substring; case-sensitive.

   :param text: Owned substring.
   :returns: A matcher object.

.. cpp:function:: auto matches_regex(std::string pattern)

   Requires a full-string match using the default std::regex syntax. Construction throws std::regex_error for an invalid pattern.

   :param pattern: Regular expression.
   :returns: A matcher object.

.. cpp:function:: auto contains_regex(std::string pattern)

   Searches for a matching substring using the default std::regex syntax. Construction throws std::regex_error for an invalid pattern.

   :param pattern: Regular expression.
   :returns: A matcher object.

Ranges
------

.. cpp:function:: auto is_empty()

   Matches an empty range.

   :returns: A matcher object.

.. cpp:function:: template <typename M> auto contains(M inner)

   Matches when at least one range element matches inner.

   :param inner: Element matcher.
   :returns: A matcher object.

.. cpp:function:: template <typename M> auto each(M inner)

   Matches when every element matches inner. Empty ranges match.

   :param inner: Element matcher.
   :returns: A matcher object.

.. cpp:function:: template <typename M> auto size_is(M inner)

   Applies inner to the size of a sized range.

   :param inner: Matcher for the range size.
   :returns: A matcher object.

.. cpp:function:: template <typename... Ms> auto elements_are(Ms... inners)

   Requires exactly one element per matcher, in order. No matchers accepts only an empty range.

   :param inners: Ordered element matchers.
   :returns: A matcher object.

.. cpp:function:: template <typename... Ms> auto unordered_elements_are(Ms... inners)

   Requires a one-to-one assignment of elements to matchers, allowing overlapping matchers. Counts must be equal. No matchers accepts only an empty range.

   :param inners: Element matchers; each must match a distinct element.
   :returns: A matcher object.

Members and alternatives
------------------------

.. cpp:function:: template <typename Member, typename M> auto field(Member member, M inner)

   Applies inner to ``std::invoke(member, actual)``.

   :param member: Data-member pointer, member-function pointer, or compatible callable.
   :param inner: Matcher for the extracted value.
   :returns: A matcher object.

.. cpp:function:: template <typename Member, typename M> auto property(Member member, M inner)

   Equivalent to field; applies inner to the invoked member or accessor.

   :param member: Invocable accessor.
   :param inner: Matcher for the extracted value.
   :returns: A matcher object.

.. cpp:function:: template <typename M> auto pointee(M inner)

   Requires a non-null pointer and applies inner to its dereferenced value.

   :param inner: Pointee matcher.
   :returns: A matcher object.

.. cpp:function:: template <typename M> auto optional_value(M inner)

   Requires an engaged optional and applies inner to its value.

   :param inner: Contained-value matcher.
   :returns: A matcher object.

.. cpp:function:: template <typename T, typename M> auto variant_with(M inner)

   Requires a variant holding T and applies inner to its value.

   :param T: Required alternative type.
   :param inner: Alternative matcher.
   :returns: A matcher object.

Floating-point values
---------------------

.. cpp:function:: template <typename T> auto near(T expected, T absolute, T relative = T{})

   Matches exact equality or a difference no larger than ``max(absolute, relative * max(abs(actual), abs(expected)))``. NaN never matches; equal infinities match.

   :param expected: Expected numeric value.
   :param absolute: Absolute tolerance.
   :param relative: Relative tolerance; defaults to zero.
   :returns: A matcher object.

.. cpp:function:: template <std::floating_point T> auto ulp_near(T expected, unsigned distance = 4)

   Matches floating-point values within the specified number of representable steps. NaN never matches; signed zeroes match.

   :param expected: Expected floating-point value.
   :param distance: Maximum ULP distance; defaults to four.
   :returns: A matcher object.

.. cpp:function:: auto is_nan()

   Matches a floating-point NaN.

   :returns: A matcher object.

Callables and exceptions
------------------------

.. cpp:function:: template <typename E> auto throws()

   Invokes a nullary callable and matches an exception caught as const E&. The overload with an inner matcher applies it to that exception. No exception or a different type mismatches. A library skip propagates.

   :param E: Expected exception type.
   :returns: A matcher object.

.. cpp:function:: template <typename E, typename M> auto throws(M inner)

   Invokes a nullary callable and matches an exception caught as const E&. The overload with an inner matcher applies it to that exception. No exception or a different type mismatches. A library skip propagates.

   :param E: Expected exception type.
   :param inner: Optional exception matcher.
   :returns: A matcher object.

.. cpp:function:: auto does_not_throw()

   Invokes a nullary callable and matches normal completion. A library skip propagates.

   :returns: A matcher object.

Custom matchers
---------------

.. cpp:function:: template <typename F> auto satisfies(F predicate, std::string description = "predicate satisfied")

   Construct a matcher from a predicate.

   :param predicate: Callable accepting const T& and returning a Boolean-like value or match_result.
   :param description: Explanation used for Boolean-like predicate results.
   :returns: matcher<F> owning both arguments.

.. cpp:struct:: match_result

   Outcome and diagnostic text returned by a matcher.

.. cpp:member:: bool match_result::matched

   Whether the value matched.

.. cpp:member:: std::string match_result::explanation

   Diagnostic text describing the comparison.

.. cpp:function:: explicit match_result::operator bool() const

   Return matched.

.. cpp:struct:: template <typename F> matcher

   Callable wrapper owning a predicate and a fallback description.

.. cpp:member:: template <typename F> F matcher<F>::predicate

   Stored predicate.

.. cpp:member:: template <typename F> std::string matcher<F>::description

   Explanation for a predicate returning a Boolean-like value.

.. cpp:function:: template <typename F> template <typename T> match_result matcher<F>::operator()(const T& value) const

   Invoke the predicate. Return its match_result unchanged, or convert its Boolean-like result and attach description.

   :param value: Value under test.
   :returns: Match outcome and explanation.
