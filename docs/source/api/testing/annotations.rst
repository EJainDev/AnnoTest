Annotations and parameter types
===============================

.. cpp:namespace:: annotest

Annotations use C++26 expression syntax: ``[[= test{}]]``.
Types below are exported by ``import annotest;``.
Annotated members must be public and callable by the runner.
Invalid annotation combinations fail discovery or compilation.

Tests and suites
----------------

.. cpp:struct:: template <fixed_string Name = ""> test

   Mark a method as a test. An empty Name uses its C++ identifier; otherwise Name supplies the displayed name.

   :var disabled: Boolean; defaults to false. Skip unless --run-disabled is active.
   :var repetitions: Unsigned invocation count; defaults to 1. Valid range is 1–1,000,000. Multiplied by --repeat, with the same limit on the product.
   :var name: Static name storage derived from Name.

.. cpp:struct:: template <fixed_string Name = ""> suite

   Configure a suite class.

   :var disabled: Boolean; defaults to false. Applies to all contained cases.
   :var lifecycle: fixture_lifecycle; defaults to per_test.
   :var name: Static name storage derived from Name.

   Fresh fixtures are constructed for each invocation under per_test. A shared fixture persists across cases in one process.

.. cpp:enum-class:: fixture_lifecycle

   Fixture reuse policy.

.. cpp:enumerator:: fixture_lifecycle::per_test

   Construct and destroy a fixture for each invocation.

.. cpp:enumerator:: fixture_lifecycle::shared

   Retain a fixture across selected cases within one process.

.. cpp:struct:: nested

   Mark a nested class as a child suite. The name and tags inherit the enclosing suite prefix. An inner constructor may accept Parent& or Parent&, test_context&. A shared child requires a shared parent.

.. cpp:struct:: timeout

   Per-case subprocess deadline annotation. Applies to a method or suite. --timeout overrides it.

.. cpp:member:: double timeout::seconds

   Positive finite deadline in seconds, including worker startup. Exceeding it kills the worker process tree and fails the case.

Lifecycle hooks
---------------

.. cpp:struct:: template <typename T = int> before_each

   Per-invocation setup. Public inherited hooks execute base-first before the body.

   Annotated hooks take no required arguments, or test_context&. A hook cannot also be a test or carry another lifecycle annotation.

.. cpp:struct:: template <typename T = int> after_each

   Per-invocation cleanup. Hooks execute in reverse order after setup or body failure; an earlier cleanup failure does not stop later cleanup.

   Annotated hooks take no required arguments, or test_context&. A hook cannot also be a test or carry another lifecycle annotation.

.. cpp:struct:: before_all

   Public static suite setup, run once before the first participating fixture is constructed.

   Annotated hooks take no required arguments, or test_context&. A hook cannot also be a test or carry another lifecycle annotation.

.. cpp:struct:: after_all

   Public static suite cleanup, run in reverse order after fixtures are destroyed. Runs for each suite whose setup was entered, including failed setup and fail-fast.

   Annotated hooks take no required arguments, or test_context&. A hook cannot also be a test or carry another lifecycle annotation.

Suite hooks receive a context named ``_suite``, retained from setup through
cleanup. Setup properties are copied to participating cases. Setup failure or
skip suppresses bodies. Cleanup failures form a separate teardown case in
ordinary execution and merge into the selected case in isolated execution.
Each worker and parameterized suite instance has its own suite-hook context.

Ordinary listing, filtered cases, disabled cases, and OS-skipped cases do not
enter suite hooks. Factory discovery may enter hooks to construct its fixture.
See :doc:`/explanation/execution` for fixture and process lifetimes.

Value parameters
----------------

.. cpp:struct:: template <typename... Ts> parameters

   One complete function or constructor argument set. Owns a structural tuple named params.

.. cpp:function:: template <typename... Ts> parameters<Ts...>::parameters(Ts... args)

   Store the supplied arguments by value.

   :param args: Compile-time structural argument values.

.. cpp:function:: template <typename... Ts> template <typename... Us> constexpr auto parameters<Ts...>::cross(const parameters<Us...>& other) const

   Return a structural tuple with one parameters set for each element of other. Every set consists of this set's arguments followed by that element.

   :param other: Values to append individually.
   :returns: Structural tuple of argument sets. An empty other produces an empty tuple.

.. cpp:struct:: template <typename... Params> parameterize

   Expand each parameters set into a concrete case with a one-based suffix. Each Params must be a specialization of parameters. May annotate a method or a suite; suite values become constructor arguments.

   :var N: Static number of parameter sets.
   :var parameters: Structural tuple of the sets.

   Construct with parameterize{parameters(...), parameters(...)}, a structural tuple of sets, or structural tuples whose contents are concatenated. Empty case sets are rejected.

Type and combined parameters
----------------------------

.. cpp:struct:: template <typename... Ts> template_params

   One complete template argument list.

.. cpp:type:: template <typename... Ts> template <typename Other> template_params<Ts...>::cross_t

   Structural tuple containing template_params<Ts..., U> for each U in Other, which must be another template_params specialization.

.. cpp:struct:: template <typename... Args> parameterize_template

   Expand template_params argument groups into concrete method or suite template specializations.

   :var N: Static number of type groups.

   Annotated method templates must accept an empty template argument list; provide default arguments. Each case still uses its supplied types. Unannotated helper templates do not require defaults.

.. cpp:function:: template <typename... Args> template <typename... Ts> static consteval auto parameterize_template<Args...>::of()

   Combine structural tuple types of template_params groups into one parameterize_template annotation.

   :returns: Annotation holding the concatenated type groups.

.. cpp:struct:: template <typename T, typename P> parameterize_pair

   Pair one template_params type T with one parameters type P. Its params member stores the value arguments.

.. cpp:struct:: template <typename... Pairs> parameterize_dual

   Expand parameterize_pair values into cases supplying both template and function arguments.

   :var N: Static number of pairs.
   :var sets: Structural tuple of pairs.

   Construct from pairs, a structural tuple of pairs, or structural tuples to concatenate.

Runtime inputs and factories
----------------------------

.. cpp:struct:: template <auto Provider> parameter_source

   Use a runtime provider returning a range of tuples or named_case values. May annotate a method or suite. Providers run during discovery; names and data must be stable across worker re-execution. Empty sets and duplicate names fail discovery. See :doc:`sources` for data types.

.. cpp:struct:: template <auto Provider = nullptr> test_factory

   Mark a method returning a range of dynamic_case values, or use an explicit free/member provider. Factory setup runs once and its fixture persists across children; cleanup runs after the children. Discovery runs the factory. Workers reconstruct it for the selected child.

Tags and naming
---------------

.. cpp:struct:: template <fixed_string Name> tag

   Attach a label to a suite or method. Suite tags are inherited. CLI --tag and --exclude-tag select labels.

.. cpp:struct:: template <auto Generator> name_generator

   Generate method display names at compile time. A method annotation overrides the suite's generator. Generator accepts std::string_view and returns a string or string view.

.. cpp:struct:: template <auto Generator> suite_name_generator

   Generate the suite display name at compile time. Generator accepts std::string_view and returns a string or string view.

.. cpp:function:: constexpr std::string readable_name(std::string_view name)

   Replace underscores in a name with spaces.

   :param name: Original name.
   :returns: Owning string containing the readable name.

Generated names must be nonempty and unique within their scope, with no tabs
or line breaks. Parameter suffixes identify each concrete case.

Runtime conditions
------------------

.. cpp:struct:: template <auto Predicate> enabled_if

   Run only when the predicate returns true. Predicates take no arguments or const test_context& and return a Boolean-convertible value. May annotate methods or suites.

.. cpp:function:: constexpr enabled_if::enabled_if(const char* reason = "Execution condition not met")

   Store a reason of at most 255 bytes. Empty reasons use the runner's default message. Longer reasons are invalid in a constant annotation.

   :param reason: Reason recorded when the condition skips a case.

.. cpp:struct:: template <auto Predicate> disabled_if

   Skip when the predicate returns true. Predicates take no arguments or const test_context& and return a Boolean-convertible value. May annotate methods or suites.

.. cpp:function:: constexpr disabled_if::disabled_if(const char* reason = "Execution condition matched")

   Store a reason of at most 255 bytes. Empty reasons use the runner's default message. Longer reasons are invalid in a constant annotation.

   :param reason: Reason recorded when the condition skips a case.

Conditions execute before fixture construction and extensions, once per
executed repetition. Inherited suite conditions precede method conditions;
evaluation stops at the first skip. Annotation-disabled and OS-skipped cases
bypass conditions. ``--run-disabled`` does not bypass them.
A predicate exception fails the case in the condition phase; skip(reason)
propagates a skip. Ordinary listing does not evaluate predicates. Factory
conditions run during discovery before fixture construction and must be
consistent during worker replay.

Operating-system restrictions
-----------------------------

.. cpp:enum-class:: operating_system

   Platform identifiers: windows, macos, linux_os, unknown.

.. cpp:struct:: template <int N> requires_os

   Run when any listed OS matches. The os member is an array of N operating_system values. Class template argument deduction infers N from the initializer. Bodies must still compile on every target platform.

.. cpp:struct:: template <int N> disallows_os

   Skip when any listed OS matches. The os member is an array of N operating_system values. Class template argument deduction infers N from the initializer. Bodies must still compile on every target platform.

Name storage
------------

.. cpp:struct:: template <std::size_t N> fixed_string

   Structural string-literal wrapper used by test, suite, and tag template arguments. N includes the terminating null character.

.. cpp:member:: template <std::size_t N> char fixed_string<N>::value[N]

   Inline copied string storage.

.. cpp:function:: template <std::size_t N> constexpr fixed_string<N>::fixed_string(const char (&str)[N])

   Copy all N characters into value.

.. cpp:function:: template <std::size_t N> static constexpr bool fixed_string<N>::is_empty()

   Return whether N equals one.
