# Testing AnnoTest

Configure with `BUILD_TESTS=ON`, build, and run CTest from the build directory:

```sh
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++-16 \
  -DBUILD_TESTS=ON -DBUILD_EXAMPLES=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests require the library's C++26 reflection compiler and Python 3. Python uses
only its standard library. `BUILD_TESTS=OFF` does not require Python.

Test design follows the behavioral patterns in GoogleTest's own self-tests:
[assertions, evaluation and diagnostics](https://github.com/google/googletest/blob/main/googletest/test/gtest_unittest.cc)
and [independently verified XML reports](https://github.com/google/googletest/blob/main/googletest/test/gtest_xml_output_unittest.py).
These are references for test design; this suite checks AnnoTest's API and semantics.
Cases are selected for distinct behavior, boundaries, failure modes, and observable
results. Repeating identical comparisons for each integer width does not provide
useful coverage, and source LOC ratios are not used as a quality measure.

The C++ executables are fixtures, including deliberate failing tests. The Python
integration tests invoke individual cases with a timeout, check exit status and
continuation, and independently parse reports with `xml.etree.ElementTree`.
Running a fixture without filters may therefore return 1; run CTest to verify
that both success and failure behavior match expectations.

| Files | Coverage |
| --- | --- |
| `assertion_tests.cpp`, `expectation_tests.cpp`, `integration/test_checks.py` | Every comparison, truth, near, containment, null, and exception helper; positive and negative paths; NaN, infinity, tolerance boundaries, empty containers, mixed types, derived/exact/nonstandard exceptions, `wrap`, source locations, and multiple failures |
| `behavior_tests.cpp`, `integration/test_behavior.py` | Exact expected/actual diagnostics, assertion unwinding, expectation continuation, mixed fatal/nonfatal failures, single operand/callback evaluation, explicit source locations, deferred/copy/reference semantics of `wrap`, noncopyable containers, and embedded-null strings |
| `generic_tests.cpp`, `integration/test_generic.py` | Custom comparable/formattable operands for all six comparisons and containment; explicit boolean proxies for comparison results and truth checks; exact custom diagnostics; single operator/conversion evaluation; lazy formatting on success; throwing operators, proxy conversions, and formatters; exception reporting, unwinding, and continuation of later cases |
| `parameterization_tests.cpp`, `integration/test_parameterization.py` | Value, template and dual annotations; multiple arguments and empty argument sets; tuple constructors; value/type cross products; `parameterize_template::of`; named/disabled cases; index selection, deduplication, ordering, and continuation |
| `annotation_tests.cpp`, `integration/test_annotations.py` | `test`, `suite`, custom/default names, disabled tests/suites, defaults, static/const methods, unannotated/private members, all OS annotations, ignored unrelated suite/method annotations, and empty suites |
| `lifecycle_tests.cpp`, `integration/test_lifecycle.py` | `before_each`/`after_each` ordering, shared state, assertion/expectation/standard/unknown exceptions in each phase, cleanup after failure, skipping, and contract state isolation |
| `contract_tests.cpp`, `integration/test_contracts.py` | Actual C++26 pre/post/inline contracts under observe semantics; all four helpers with void/value returns; failures, callable exceptions, flag reset, and custom handler delegation |
| `death_tests.cpp`, `integration/test_death.py` | Both death helpers with nonzero exit, exceptions and signals; normal return/zero exit failures and child state isolation (Unix only) |
| `fault_tests.cpp`, `integration/test_faults.py` | Linux fault injection for partial capture initialization, descriptor cleanup/restoration, and fork/wait errors; coverage-only child counter flushing |
| `internal_tests.cpp`, `integration/test_internals.py` | Private log state, absent optional diagnostics, exception formatting, unrecorded assertions, and null CLI pointers without exporting internals; compile-time signed/unsigned integer formatting including zero and 64-bit limits |
| `utility_tests.cpp` | Enum formatting, invalid values, and the shipped CMake discovery helper |
| `integration/test_cli.py` | Long/short/grouped/attached/repeated CLI options, filtering, unknown names, malformed arguments, help/list, and terminator |
| `integration/test_reporting.py` | XML totals, timings, timestamp, source file, failure types, escaping, stdout/stderr including C stdio and large writes, capture isolation, report overwrites, file errors, and repeated-run reset |
| `compile_fail/*.cpp`, `expect_compile_failure.cmake` | Rejection of duplicate lifecycle hooks and test/lifecycle combinations in both annotation orders; `assertion_error` remains inaccessible to importers |

CTest compile-failure cases are serialized because each invokes the build
system. They require a diagnostic from the annotation validator, not merely a
nonzero compiler exit. All temporary report paths are isolated per invocation,
so individual runtime tests can run concurrently. Helper and behavior scenarios
are registered separately in CTest; use `ctest -N` to inspect them.

These tests cover the implemented API. Historical examples and docs also mention
`before_all`, `after_all`, `Abort`, and legacy camel-case names which are absent
from the current library; they are not treated as implemented features. OS
filtering is tested against the host platform. Fork/waitpid resource failures
cannot be induced deterministically by this suite.

The GCC 16.2 coverage run on 2026-10-11 passed all 195 tests and reached 100%
reachable runtime source-associated branch coverage (346/346), with 98.6% runtime
line coverage. Raw compiler branch coverage is 65.0%; it includes call and cleanup
arcs that are outside the agreed source-decision metric. The coverage gate is
`tools/coverage_audit.py`, also run in CI. Local data and counts are under
`build/coverage/`. Constant evaluation and compile-failure checks are not runtime
coverage. Cross-platform coverage has not been established.

Source branch coverage measures executed decisions in the library, not coverage
of all template instantiations or supported C++ types. The generic fixtures add
behaviorally distinct custom operands, formatters, and proxy conversions; they
do not establish comprehensive template-type coverage. The percentages and
195-test count above describe the earlier run, before these fixtures were added.

Compile-time checks explicitly instantiate both signed and unsigned paths of the
internal integer formatter. Unrelated template annotations on suites and methods
exercise the ignored-annotation alternatives, including either order around a
`test` annotation and an unrelated annotation without `test`. The false path of
`run_discovered_test`'s `Test.parameterized` guard is not instantiated: normal
dispatch only assigns that callback to parameterized tests, and calling it for a
nonparameterized test would fall off a non-void function. These checks are not
counted as runtime source branches.

Package integration is checked separately after the producer build:

```sh
python3 tools/check_installation.py --build-dir build
```

The installed and FetchContent consumers in `tests/consumer` exercise the public
link target, contract-handler header, assertions, parameters, and CTest discovery.
The installation is relocated to a path containing spaces and its exports are
checked for source/build paths. CI configures non-default include/library directories
before running this check. `--cmake /path/to/cmake` selects a CMake version for the
consumer checks; CMake 4.3 requires reflection on its shared standard-library module,
which the consumer configures after `project()` and before defining targets.
