Matchers
========

All matchers are in the ``annotest`` namespace. Pass a value and a matcher to
``assert_that(actual, matcher, message = "")`` or
``expect_that(actual, matcher, message = "")``. Both accept an optional
``std::source_location`` after the message, defaulted to the call site.
A failed assertion unwinds the current body; a failed expectation continues.

.. code-block:: cpp

    assert_that(7, all_of(gt(0), lt(10)));
    expect_that(std::string{"ready"}, starts_with("rea"), "service status");

Comparison and composition
--------------------------

.. list-table:: Value matchers
   :header-rows: 1
   :widths: 40 60

   * - Matcher
     - Matches when
   * - ``eq(expected)``, ``ne(expected)``
     - The value is equal or unequal to ``expected`` using its comparison operators.
   * - ``lt(bound)``, ``le(bound)``, ``gt(bound)``, ``ge(bound)``
     - The value satisfies the corresponding ordering relation with ``bound``.
   * - ``anything()``
     - Always.
   * - ``is_true()``, ``is_false()``
     - Conversion of the value to bool produces true or false.
   * - ``is_null()``, ``not_null()``
     - The value compares equal or unequal to ``nullptr``.
   * - ``all_of(m1, m2, ...)``
     - Every matcher succeeds; evaluation stops at the first mismatch.
   * - ``any_of(m1, m2, ...)``
     - At least one matcher succeeds; evaluation stops at the first match.
   * - ``negate(inner)``
     - ``inner`` does not match.

``all_of()`` with no arguments matches every value; ``any_of()`` with no
arguments matches none. ``eq`` compares C-string pointers by identity. Use
:doc:`string assertions <assertions>` for C-string content equality.

Strings and regular expressions
-------------------------------

``starts_with(prefix)``, ``ends_with(suffix)``, and ``has_substr(text)`` accept
narrow strings and string views. Matching is case-sensitive.
``matches_regex(pattern)`` requires the entire string to match;
``contains_regex(pattern)`` searches for a matching substring.
Patterns use the default ``std::regex`` syntax. Invalid patterns throw
``std::regex_error`` when the matcher is constructed.

.. code-block:: cpp

    assert_that(std::string{"item-42"}, matches_regex("item-[0-9]+"));
    expect_that(std::string{"count: 42"}, contains_regex("[0-9]+"));

Ranges
------

.. list-table:: Range matchers
   :header-rows: 1
   :widths: 40 60

   * - Matcher
     - Behavior
   * - ``is_empty()``
     - Checks whether the range is empty.
   * - ``contains(inner)``
     - Matches if at least one element matches ``inner``.
   * - ``each(inner)``
     - Requires every element to match. An empty range matches.
   * - ``size_is(inner)``
     - Applies ``inner`` to the size of a sized range.
   * - ``elements_are(m1, m2, ...)``
     - Requires exactly one element per matcher, in the specified order.
   * - ``unordered_elements_are(m1, m2, ...)``
     - Requires exactly one distinct element per matcher, in any order.

``unordered_elements_are`` supports overlapping matchers. Each matcher must
match a different element; an element cannot satisfy multiple positions.
Both element-list matchers with no arguments match only empty ranges.

.. code-block:: cpp

    assert_that(std::vector{1, 2, 3}, elements_are(eq(1), eq(2), eq(3)));
    assert_that(std::vector{2, 1}, unordered_elements_are(gt(0), eq(1)));

Members, pointers, and alternatives
------------------------------------

``field(member, inner)`` applies ``inner`` to the result of invoking ``member``
on the object. It accepts a data-member pointer, member-function pointer, or
compatible callable. ``property(member, inner)`` has the same behavior.
``pointee(inner)`` requires a non-null pointer and matches the pointed-to value.
``optional_value(inner)`` requires an engaged optional and matches its value.
``variant_with<T>(inner)`` requires the variant to hold ``T`` and matches that value.

.. code-block:: cpp

    struct Item { int count; };
    assert_that(Item{3}, field(&Item::count, gt(0)));
    assert_that(std::optional{7}, optional_value(eq(7)));
    assert_that(std::variant<int, std::string>{7}, variant_with<int>(eq(7)));

Numeric tolerance
-----------------

``near(expected, absolute, relative = 0)`` matches exact equality or a difference
no greater than ``max(absolute, relative * max(abs(actual), abs(expected)))``.
Supply ``expected`` and both tolerances with the same numeric type.
Negative tolerances throw ``std::invalid_argument``. Unequal nonfinite operands
do not match; equal infinities match, and NaN never matches.

``ulp_near(expected, distance = 4)`` accepts a floating-point expected value and
matches values within ``distance`` representable floating-point steps. A distance
of zero requires exact equality. Equal infinities match; unequal nonfinite values
and NaN do not. ``is_nan()`` checks for NaN directly.

.. code-block:: cpp

    assert_that(100.5, near(100.0, 0.01, 0.01));
    assert_that(std::nextafter(1.0, 2.0), ulp_near(1.0, 1));

Exceptions
----------

``throws<E>()`` invokes a callable and matches an exception of type ``E`` or a
derived type. ``throws<E>(inner)`` also applies ``inner`` to the caught exception.
Returning normally or throwing another exception type does not match.
``does_not_throw()`` matches a callable that returns normally.
These matchers propagate ``skip(reason)`` as a test skip.

.. code-block:: cpp

    assert_that([] { throw std::runtime_error("invalid row"); },
                throws<std::runtime_error>(property(
                    [](const std::runtime_error& error) { return error.what(); },
                    has_substr("row"))));

Custom matchers
---------------

``satisfies(predicate, description = "predicate satisfied")`` wraps a callable
that accepts the tested value and returns a bool-convertible value or ``match_result``.
``match_result`` contains ``bool matched`` and ``std::string explanation``.
A failed check includes the explanation in its diagnostic.
Calling a matcher directly returns ``match_result`` without recording a failure.

.. code-block:: cpp

    auto even = satisfies([](int value) { return value % 2 == 0; }, "an even number");
    assert_that(4, even);
    assert_true(even(6).matched);
