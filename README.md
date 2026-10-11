# AnnoTest

[![Build and Test](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml)
[![Documentation](https://readthedocs.org/projects/annotest/badge/?version=stable)](https://annotest.readthedocs.io/)
[![License: MIT](https://img.shields.io/badge/license-MIT-lightgray)](LICENSE)

AnnoTest is a C++26 unit testing library built with reflection, annotations, and modules.
Mark public methods in a suite with `[[= test{}]]`, then explicitly run the suite:

```cpp
import annotest;
using namespace annotest;

struct ArithmeticTests {
    [[= test{}]] void addition() { assert_equal(4, 2 + 2); }
};

int main(int argc, char** argv) {
    return run_tests<ArithmeticTests>(argc, argv);
}
```

The runner discovers annotated methods at compile time. It supports named and disabled
suites/tests, per-test hooks, value and template parameterization, fatal assertions,
nonfatal expectations, exception checks, process death checks, and XML reports.

The API is under development. This README describes the current source branch;
published documentation and older release tags may describe an earlier API.

## Requirements

AnnoTest implements Linux, macOS, and native Windows backends. Use **GCC 16 or
newer, CMake 4.3.3 or newer, and Ninja**, with C++26 reflection, annotations,
standard-library modules, and contracts. CI includes native macOS and Windows
jobs alongside the pinned Linux GCC container. Tagged releases also build macOS
ARM64 and Windows x86-64 installation archives after testing each native build.

- CMake minimum: 4.3.3. The experimental `import std` setting is version-sensitive.
- C++ standard: 26, with `-freflection` propagated by the library target.
- Python 3 is required only when `BUILD_TESTS=ON`.
- StructuralTuple and SerialXML are found as installed packages or fetched at pinned
  commits during configuration. Fetching requires Git and network access.
- macOS uses Homebrew GCC; Apple Clang does not provide the required reflection support.
- Windows uses MSYS2 UCRT64 GCC and produces native `.exe` files. MSVC is not a
  supported compiler for these C++26 features.
- Stack traces use `std::stacktrace` when available, otherwise native backtraces
  (raw frame addresses on Windows).

The [development container](.devcontainer/devcontainer.json) provides GCC 16.2 and CMake.
Use Ninja for the commands below.

## Native platform setup

On macOS, install [Homebrew GCC](https://formulae.brew.sh/formula/gcc):

```sh
brew install gcc cmake ninja python
```

Use `-DCMAKE_CXX_COMPILER=g++-16` in the build command below.

On Windows, install [MSYS2](https://www.msys2.org/) and open its **UCRT64** shell:

```sh
pacman -Syu
# Reopen UCRT64 if the updater asks, then install the native tools:
pacman -S --needed git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-python
```

Run the build commands from UCRT64. Keep `C:\msys64\ucrt64\bin` on `PATH` when
running executables outside that shell so GCC runtime DLLs can be found. Example
executables have an `.exe` suffix on Windows. Python tests use UTF-8 report output.

## Build and run

From the repository root, with the supported toolchain available:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Use your GCC compiler path if `g++` selects another version. Examples are built by
default for a standalone checkout; embedded builds default to the library only.
Self-tests are opt-in. To build only the library, set `BUILD_TESTS=OFF` and
`BUILD_EXAMPLES=OFF`.

```sh
./build/examples/example1_basic
./build/examples/example3_parameterized --list
./build/examples/example3_parameterized --test-name ParameterizedTests.addition.2
./build/examples/example10_reporting --xml-output report.xml
```

A normal run returns `0` when its executed tests pass and `1` on test failures.
Invalid CLI arguments, unknown test names, and report write failures also return `1`.
Self-test fixtures include deliberate failures checked by independent Python tests;
use CTest to run the self-test suite.

## Examples

Each example has a runnable `main`. With both build options enabled, CTest runs all ten
examples in addition to the library's self-tests.

| Source | Executable | Demonstrates |
| --- | --- | --- |
| [Basic tests](examples/example01_basic.cpp) | `example1_basic` | Checks, names, exception assertions |
| [Lifecycle](examples/example02_lifecycle.cpp) | `example2_lifecycle` | Fixture reset and cleanup around every instance |
| [Value parameters](examples/example03_parameterized.cpp) | `example3_parameterized` | Argument sets, expected results, value cross products |
| [OS annotations](examples/example04_os_annotations.cpp) | `example4_os_annotations` | Required and disallowed platforms |
| [Death tests](examples/example05_death_tests.cpp) | `example5_death_tests` | Child exit, exceptions, process isolation |
| [Contracts](examples/example06_contracts.cpp) | `example6_contracts` | Pre/post/inline checks under observe semantics |
| [Stack fixture](examples/example07_full_suite.cpp) | `example7_full_suite` | A named suite, hooks, skips and parameterization |
| [Typed tests](examples/example08_typed.cpp) | `example8_typed` | Template argument sets and type cross products |
| [Type and value parameters](examples/example09_dual_parameters.cpp) | `example9_dual_parameters` | Coupled template and function arguments |
| [Reporting and failures](examples/example10_reporting.cpp) | `example10_reporting` | Captured output, XML, fatal/nonfatal behavior |

The reporting example passes normally. To deliberately produce failures:

```sh
ANNOTEST_DEMONSTRATE_FAILURE=1 ./build/examples/example10_reporting --xml-output failures.xml
```

This command returns `1`. The report contains two expectation failures and one assertion
failure in `failure_behavior`. The body continues after the expectations, unwinds at
the assertion, and the runner still executes `later_test`.

## Writing tests

Use `[[= test<"custom name">{}]]` to name a method, or
`[[= test{.disabled = true}]]` to disable it. A suite can likewise use
`struct [[= suite<"custom suite">{}]] MyTests` or `suite{.disabled = true}`.
Test and hook methods must be public. Inaccessible or deleted annotated methods,
hooks with required arguments, tests missing parameterization, empty parameter
sets, duplicate display names, and names with tabs or line breaks are rejected
at compile time. Private unannotated helpers and fixture data are allowed.

`before_each` and `after_each` run around each executed test, including each
parameterized instance. One suite object is created per `run_tests` call, so member
state persists unless setup resets it. Cleanup still runs after setup or body exceptions.
There are no `before_all` or `after_all` annotations.

```cpp
struct FixtureTests {
    int value = 0;
    [[= before_each{}]] void setup() { value = 10; }
    [[= after_each{}]] void cleanup() { value = 0; }
    [[= test{}]] void increment() { assert_equal(11, ++value); }
};
```

A failed `assert_*` records a failure and unwinds the current body. A failed `expect_*`
records a failure and continues at the next statement. Both make the run return `1`;
later tests still execute. Uncaught exceptions and unexpected contract violations
also fail the test. Disabled or OS-filtered tests do not run their hooks.

| Purpose | Assertion | Expectation |
| --- | --- | --- |
| Equality | `assert_equal(expected, actual)` | `expect_equal(expected, actual)` |
| Inequality | `assert_not_equal(expected, actual)` | `expect_not_equal(expected, actual)` |
| Boolean | `assert_true`, `assert_false` | `expect_true`, `expect_false` |
| Numeric tolerance | `assert_near(expected, actual, tolerance)` | `expect_near(expected, actual, tolerance)` |
| Ordering | `assert_less`, `assert_less_equal`, `assert_greater`, `assert_greater_equal` | Corresponding `expect_*` functions |
| Containment | `assert_contains(container, value)` | `expect_contains(container, value)` |
| Null pointer | `assert_null(pointer)` | `expect_null(pointer)` |
| Exception | `assert_throws<E>(callable)`, `assert_throws_exact<E>(callable)` | `expect_throws<E>`, `expect_throws_exact<E>` |
| Contract | `assert_contract_violation`, `assert_no_contract_violation` | Corresponding `expect_*` functions |
| Process death | `assertDeath(callable)` | `expectDeath(callable)` |

Death helpers retain their camel-case names. A thrown exception in the child counts as
death; normal return or exit status zero does not. Linux and macOS use `fork` and
`waitpid`. Windows relaunches the executable, selects the current test, and replays
its setup and body up to the requested death check. Earlier death callables are
skipped during replay; multiple checks and parameterized cases are supported.

On Windows, use death checks inside `run_tests` and forward `argc`/`argv` as shown
above. Setup and the body before the death check must be deterministic and safe to
repeat. State from previously executed tests, parent-only resources, and pointers
into another process are not inherited. A child that fails before reaching the
callable reports a failed death check. Output capture is process-wide on all
platforms, so test bodies should finish background writers before returning.

For contract checks, include `<annotest_contract.h>` in exactly one translation unit
per executable and enable `-fcontracts -fcontract-evaluation-semantic=observe` on the
code containing contracts. Observe semantics continue after a violation: the body
must remain safe even with invalid input. See the contract example for safe cases.

## CLI and reports

| Option | Behavior |
| --- | --- |
| `-h`, `--help` | Print usage and exit |
| `-l`, `--list` | List the suite and test names |
| `-t`, `--test-name NAME...` | Run fully qualified names; repeatable |
| `--xml-output FILE` | Write a suite XML report, overwriting the file |

Fully qualified names are `Suite.Test`. Parameterized instances append a one-based
index, such as `ParameterizedTests.addition.2`. Selecting the base method name runs
all its instances. Quote names containing spaces.

Reports include timings, failures, skip totals, and captured stdout/stderr. Test output
is captured even without an XML file; use `--xml-output` to inspect it. Skipped tests
currently contribute to totals without individual `<testcase>` elements. The runner
uses shared state and process-wide output capture; use separate processes for concurrent
runs rather than invoking runners concurrently or recursively.

## CMake integration

This consumer project assumes a checkout of this source revision at `../AnnoTest`,
and the first test source above saved as `test.cpp`:

```cmake
cmake_minimum_required(VERSION 4.3.3)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS 4.4)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
else()
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
endif()
set(CMAKE_CXX_MODULE_STD ON)
project(MyTests LANGUAGES CXX)
if(CMAKE_VERSION VERSION_LESS 4.4)
  # Enable reflection on CMake 4.3's shared std module after compiler detection.
  string(APPEND CMAKE_CXX_FLAGS " -freflection")
endif()

enable_testing()
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(../AnnoTest annotest-build)

add_executable(my_tests test.cpp)
target_link_libraries(my_tests PRIVATE annotest::annotest)
annotest_discover_tests(my_tests)
```

Configure with `cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++`, build, then
run `ctest --test-dir build --output-on-failure`. The `import std` setup must precede
`project()` in the consumer, too. The UUIDs match the tested CMake series;
future versions may need a different experimental setting. CMake 4.3 also requires
`-freflection` in the top-level `CMAKE_CXX_FLAGS` after `project()` so its shared `std` module
contains reflection declarations. CMake 4.4 propagates these options through targets.

For FetchContent, replace `add_subdirectory` with the following, substituting a
published commit containing this API for `<commit>`:

```cmake
include(FetchContent)
FetchContent_Declare(annotest
  GIT_REPOSITORY https://github.com/EJainDev/AnnoTest.git
  GIT_TAG <commit>)
FetchContent_MakeAvailable(annotest)
```

Older v1.0 tags may not contain this API. Source builds expose both `annotest` and
`annotest::annotest`; installed packages expose `annotest::annotest`.

To install a standalone library build into a local prefix:

```sh
cmake -S . -B build/install -G Ninja \
  -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
cmake --build build/install --parallel 2
cmake --install build/install --prefix "$PWD/install"
```

Fetched dependencies are installed into the same prefix. If dependencies were found
as preinstalled packages instead, their prefixes must also be available to the consumer.
The installed contract handler is available as `<annotest_contract.h>`.

In the consumer CMake project above, replace `add_subdirectory` with
`find_package(AnnoTest CONFIG REQUIRED)`. Keep the compiler, standard-module setup,
executable, link target and discovery call. Configure and run with:

```sh
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/install
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

The installation can be moved to another prefix; point CMake at the new location.
For a custom layout, configure the producer with `CMAKE_INSTALL_INCLUDEDIR` and
`CMAKE_INSTALL_LIBDIR`. If that library directory is outside CMake's default search
layout, pass `-DAnnoTest_DIR=/new/prefix/<libdir>/cmake/AnnoTest` instead of relying
on `CMAKE_PREFIX_PATH`. Bundled dependency packages are resolved beside AnnoTest's
package configuration. Use the same compiler and compatible build settings; CMake
rebuilds module interfaces for the consumer, and the static libraries remain toolchain-specific.

To verify a build's installation and FetchContent integration:

```sh
python3 tools/check_installation.py --build-dir build/install
```

This installs into an isolated prefix, moves it to a path containing spaces, checks
exports for source/build path leaks, and builds external consumers exercising
assertions, parameterization, contracts and CTest discovery. CI also runs it with
non-default include and library directories.

The discovery helper runs the built executable with `--list` and registers each case
in CTest. Discovery failures are build errors: a failed or timed-out `--list`, malformed or
empty output, and duplicate case names cannot produce a successful manifest.
CTest reports missing executables and manifests with a build instruction. Quotes,
semicolons, dollar signs, brackets, and surrounding spaces in names are preserved.
Set `annotest_discover_tests(my_tests DISCOVERY_TIMEOUT 60)` to change the default
30-second listing timeout. Multi-config generators require `ctest -C Debug` or
`-C Release` and keep each configuration's discovery separate.

## Documentation and contributing

See the [documentation](https://annotest.readthedocs.io/), runnable examples above,
and [self-test guide](tests/README.md). Build the documentation locally with:

```sh
python3 -m venv .venv-docs
.venv-docs/bin/python -m pip install -r docs/requirements.txt
.venv-docs/bin/python -m sphinx -b html docs/source docs/build/html
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidance and [LICENSE](LICENSE)
for the MIT license.
