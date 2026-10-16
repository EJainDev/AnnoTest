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

With GCC 16 available as `g++`, use the dedicated Debug test preset:

```sh
cmake --preset test-debug-gcc-16-1
cmake --build --preset build-test-debug --parallel 2
ctest --preset test-debug
```

The `test-release` test preset selects the corresponding Release build.

The C++ executables are fixtures, including deliberate failing tests. The Python
integration tests invoke individual cases with a timeout, check exit status and
continuation, and independently parse reports with `xml.etree.ElementTree`.
Running a fixture without filters may therefore return 1; run CTest to verify
that both success and failure behavior match expectations.

The coverage audit is `tools/coverage_audit.py`. Linux CI enforces `--minimum-percent 95`
and uploads its JSON report. The default threshold is 100%; `--report-only` disables
the threshold for local inspection. It measures reachable runtime
source-associated branches, excluding documented compiler and
unreachable cleanup outcomes. Constant evaluation and compile-failure tests are
separate checks. Coverage of one platform or template specialization does not
establish coverage of every supported platform or C++ type.

Package integration is checked separately after the producer build:

```sh
python3 tools/check_installation.py --build-dir build
```

The installed and FetchContent consumers in `tests/consumer` exercise the public
link target, contract-handler header, assertions, parameters, death checks, and CTest discovery.
The check verifies relocated installation paths and exported CMake configuration.
Use `--cmake /path/to/cmake` to select a CMake version for the consumer checks.

The V1 integration category covers static suite hooks with fresh/shared/nested
fixtures, repetitions, factory discovery, registered fixtures, helper-thread
listeners, and subprocess cleanup failures. Eight compile-failure fixtures check
invalid suite-hook declarations. The example gallery also runs through CTest,
including a separate parallel-worker invocation.

JSON discovery (`--list-json`) and execution reports carry schema version 1.
The downloadable schemas live in `docs/source/_static/`; schema validation is an
additional check and does not add dependencies to the standard-library-only
Python integration tests.

The benchmark gate has independent Python tests for complete and incomplete
matrices, equality, stale library, workload, and tool hashes, malformed samples, and
ratios calculated from raw data. Run them directly with
`python3 tests/test_benchmark_gate.py`, or through CTest. Compilation performance
is measured separately from the functional suite; see
[the benchmark guide](../benchmarks/README.md) for reproduction commands.
