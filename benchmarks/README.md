# Compilation and runtime comparison with GoogleTest

Generate matching test workloads for AnnoTest and GoogleTest: the same
number of tests, fixture classes, setup/teardown operations, and one integer
equality assertion per test. Each executable checks that all expected test bodies
ran and returned success before measurements begin. These are performance
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

Both executables are built and validated before timing begins. Compiler samples
are paired, alternating which framework compiles first in each pair and each
workload. The JSON records the exact sample order. This limits the effect of CPU
load changing during a comparison.

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
a fresh installation measurement. With an installed GoogleTest package, its
static library remains prebuilt while AnnoTest and its fetched dependencies
rebuild from source; the clean comparison reflects those different build setups.
Clean builds are serial by default; use
`--jobs N` explicitly for a parallel build comparison. Their peak RSS is the
largest child process reported by `wait4`, not total memory across simultaneous
compilers. Clean builds can take substantially longer than warm compilations.


## V1 workload coverage

The runner measures three workloads selected with `--workloads`:

- `regular`: independent test methods with setup, cleanup, and integer equality.
- `parameterized`: the same number of concrete invocations supplied as value parameters.
- `output`: regular tests writing 1 MiB per invocation, exercising AnnoTest's capture and retained output.

All workloads validate the number of executed bodies. Both frameworks use fresh
fixtures. Runtime measurements launch the complete executable, including startup,
discovery, fixtures, assertions, reporting, and shutdown. Stdout is discarded by
the harness. AnnoTest still captures each test's output internally; GoogleTest's
normal runner writes it directly. The output workload therefore measures each
framework's default behavior, with different output retention semantics.

Use `--runtime-repetitions 5` to choose the runtime sample count. Runtime wall
time and process peak RSS are saved separately from compiler wall time/RSS.
RSS includes process launch overhead; small-run high-water marks may reflect
the harness launch footprint. These small runs include process startup and should not be interpreted as
per-assertion latency or parallel-worker performance. AnnoTest invokes one runner
per fixture class; GoogleTest invokes
one global runner. Runtime results include that difference. The complete
generated sources and commands are identified by the recorded hashes. No XML/JSON files are
requested in these workloads.

For a Release run covering warm compilation, parameterization, output,
and runtime, followed by a separate clean-build measurement:

```sh
python3 benchmarks/run.py --build-dir build/benchmarks-release \
  --sizes 1 24 72 240 --repetitions 5 --runtime-repetitions 5 \
  --output benchmarks/results-release-warm.json
python3 benchmarks/run.py --build-dir build/benchmarks-release \
  --sizes 1 --workloads regular --repetitions 3 --cold \
  --output benchmarks/results-release-clean.json
```

## Release compile-time gate

The release gate requires AnnoTest's median compilation time to be no greater
than GoogleTest's in every warm workload at 1, 24, 72, and 240 cases, in both
Debug and Release. Collect at least three samples per measurement on an idle
machine. Clean framework builds are reported separately from compilation of
user test files; their comparison includes the build setup described above.

Pass the two complete warm reports to:

```sh
python3 benchmarks/check_results.py \
  benchmarks/results-release-warm.json benchmarks/results-debug-warm.json
```

The checker recalculates medians from raw samples and rejects missing matrix
entries, nonfinite or nonpositive timings, fewer than three samples, and library
or workload source hashes that differ from the current checkout. Any ratio above
1.0 fails. The [generated C++ workloads](workloads/), [generator](generate.py),
[measurement harness](run.py), and [gate](check_results.py) are included in this
directory. Workload snapshots use the default three fixtures and must match the
generated sources used for release measurements.
Harness and generator hashes must also match the measurement reports.
Add `--include-clean` and supply the two clean reports to enforce the same ratio
for one-case ordinary clean builds as an additional check.
