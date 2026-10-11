# Compile-time comparison with GoogleTest

Generate matching regular-test workloads for AnnoTest and GoogleTest: the same
number of tests, fixture classes, setup/teardown operations, and one integer
equality assertion per test. Each executable checks that all expected test bodies
ran and returned success before measurements begin. These are compile-time
benchmarks, not a new correctness test suite.

## Run in the devcontainer

From the repository root (Linux, GCC with C++26 reflection, CMake 4.3.3+, Ninja,
and Python 3 are required):

```sh
cmake -S . -B build/benchmarks-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF \
  -DBUILD_BENCHMARKS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_COMPILER_LAUNCHER=
python3 benchmarks/run.py --build-dir build/benchmarks-debug \
  --sizes 1 24 72 240 --repetitions 5
```

Repeat with `-DCMAKE_BUILD_TYPE=Release` in a separate build directory to measure
optimized builds. To change sizes or fixture count, configure with
`'-DANNOTEST_BENCHMARK_SIZES=12;72;480'` and
`-DANNOTEST_BENCHMARK_SUITES=3`, then pass matching `--sizes` to the runner.
A size smaller than the fixture count uses one fixture per test.

GoogleTest uses an installed CMake package when available; otherwise FetchContent
fetches the pinned [v1.17.0 commit](https://github.com/google/googletest/commit/52eb8108c5bdec04579160ae17225d66034bd723).
For an existing source checkout, pass
`-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/usr/src/googletest` when no installed
package is available. The JSON records the installed package's version, or the
fallback version. Source overrides are labeled separately: record their actual
revision alongside results.

## What the numbers mean

The default measurement recompiles just the generated test translation unit with
all framework modules/libraries and the standard library module already built.
It excludes dependency downloads, configuration, linking, dependency scanning,
and framework compilation. Compiler launchers `ccache` and `sccache` are removed
from warm measured commands; clean builds honor CMake launchers, which the
configure command above disables. The OS filesystem cache is not flushed. Run
on an idle machine, without concurrent builds.

Both test targets use the same compiler, GNU C++26 language mode, and build-type
flags. AnnoTest additionally requires `-freflection`, module mapper inputs, and
its normal transitive dependency options. GoogleTest uses its ordinary header
interface. Neither target uses a precompiled header or unity build. This models
normal consumption of each library rather than making their internals identical.

`results.json` in the build's `benchmarks/` folder contains every wall-time and
peak compiler RSS sample, medians, object size, compiler/version, GoogleTest
version, Git revision/status when available, module and generated source hashes,
and exact compile commands. A container mounting a Git worktree without its
parent repository may have null Git metadata; module hashes still identify the
source content. Object sizes include debug information in Debug builds. The reported ratio is AnnoTest median divided by
GoogleTest median: 1.0 means equal compile time.

Add `--cold` to measure clean target builds including framework dependencies,
standard library module compilation where needed, dependency scanning, and
linking. Dependencies have already been downloaded and configured; this is not
a fresh installation measurement. Clean builds are serial by default; use
`--jobs N` explicitly for a parallel build comparison. Their peak RSS is the
largest child process reported by `wait4`, not total memory across simultaneous
compilers. Clean builds can take substantially longer than warm compilations.

The workloads cover regular tests only. Parameterization, failure diagnostics,
XML output, and runtime speed are separate questions. GoogleTest reconstructs
its fixture for each test; AnnoTest reuses its suite instance. Setup resets the
state in this workload, so the assertions and executed-test counts match, but
this benchmark does not claim identical fixture lifecycle semantics.
