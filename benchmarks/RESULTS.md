# Recorded performance results

## V1 Release measurements — October 12, 2026

Measured in the local devcontainer with GCC 16.2.0, CMake 4.4.3, GNU C++26,
Release (`-O3 -DNDEBUG`), and GoogleTest 1.15.0 (installed). Warm compilation is the
median of three direct compiler invocations with dependencies prebuilt; runtime
is the median of five whole-executable runs. Every executable verifies that all
expected test bodies executed. Validation builds finished before measurement.

Both frameworks use fresh fixtures and matching case counts and integer checks.
AnnoTest uses a separate runner call per fixture class, preserving the original
generator layout; GoogleTest uses one global runner. Runtime includes startup,
discovery, output handling, and shutdown. Stdout is discarded by the harness.

### Warm compilation

| Workload | Cases | AnnoTest (s) | GoogleTest (s) | AnnoTest / GoogleTest |
| --- | ---: | ---: | ---: | ---: |
| Ordinary | 1 | 6.396 | 2.056 | 3.11× |
| Ordinary | 24 | 9.879 | 3.093 | 3.19× |
| Ordinary | 240 | 31.983 | 14.198 | 2.25× |
| Value parameters | 1 | 6.749 | 3.286 | 2.05× |
| Value parameters | 24 | 9.971 | 4.861 | 2.05× |
| Value parameters | 240 | 23.746 | 5.094 | 4.66× |
| 1 MiB output per case | 1 | 6.385 | 2.063 | 3.10× |
| 1 MiB output per case | 24 | 9.957 | 3.078 | 3.24× |
| 1 MiB output per case | 240 | 33.138 | 14.180 | 2.34× |

### Whole-executable runtime

| Workload | Cases | AnnoTest (ms) | GoogleTest (ms) | AnnoTest RSS (MiB) | GoogleTest RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| Ordinary | 1 | 3.285 | 2.053 | 18.6 | 18.6 |
| Ordinary | 24 | 6.289 | 2.193 | 18.6 | 18.6 |
| Ordinary | 240 | 15.340 | 3.504 | 18.7 | 18.7 |
| Value parameters | 1 | 2.358 | 2.361 | 18.7 | 18.7 |
| Value parameters | 24 | 5.587 | 2.491 | 18.7 | 18.7 |
| Value parameters | 240 | 15.710 | 4.514 | 18.7 | 18.7 |
| 1 MiB output per case | 1 | 8.792 | 2.705 | 18.7 | 18.7 |
| 1 MiB output per case | 24 | 93.254 | 4.941 | 30.1 | 18.7 |
| 1 MiB output per case | 240 | 701.701 | 17.295 | 245.6 | 18.7 |

RSS is the harness child-process high-water mark, including launch overhead.
Small runs show a roughly 19 MiB measurement floor in this environment.

The output workload emits 1 MiB per case: 240 cases produce 240 MiB per run.
AnnoTest captures and retains output for reports; GoogleTest writes output
directly. These figures measure the default behaviors and their different
retention costs. They do not measure per-assertion latency or parallel workers.

### Clean target builds

Median of three serial clean builds of the one-case ordinary workload, including
compilation, dependency scanning, and linking. Dependencies were already fetched
and configured. AnnoTest and its dependencies rebuild from source; GoogleTest
links the installed prebuilt library. The comparison reflects these build setups.

| Framework | Clean build (s) | Peak child RSS (MiB) |
| --- | ---: | ---: |
| AnnoTest | 108.249 | 675.9 |
| GoogleTest | 2.224 | 267.7 |

In these Release workloads AnnoTest has higher warm compilation costs than
GoogleTest. Value-parameterized AnnoTest cases compile faster than the equivalent
number of independent AnnoTest methods at 240 cases, while remaining slower than
the measured GoogleTest parameterized workload. The earlier warm Debug figures
below describe a different source snapshot and optimization mode.

Raw samples, compiler RSS, object sizes, exact commands, and source hashes:
[warm/runtime JSON](results-v1-release.json), [clean-build JSON](results-v1-clean-release.json).
The source hashes identify the uncommitted implementation measured here.
See [README.md](README.md) for reproduction commands and scope.

## Earlier Debug measurements

Measured in the devcontainer with GCC 16.2.0, GNU C++26, Debug (`-g`), and
installed GoogleTest 1.15.0. Each number is the median of three direct compiler
invocations on an otherwise idle container, with dependencies already built.
The same generated regular-test source, setup/teardown, and equality assertions
were used before and after the runtime-module split and CLI parser replacement.
There are three fixtures per workload, except the one-test workload uses one.

| Tests | AnnoTest before | AnnoTest after | GoogleTest after | After / GoogleTest |
| ---: | ---: | ---: | ---: | ---: |
| 1 | 4.133s | 1.784s | 2.204s | 0.81× |
| 24 | 4.285s | 2.037s | 2.511s | 0.81× |
| 72 | 4.783s | 2.204s | 3.224s | 0.68× |
| 240 | 6.029s | 3.330s | 5.136s | 0.65× |

The ratio is AnnoTest divided by GoogleTest; below 1.0 means faster in this
workload. The original before snapshot also contains its own GoogleTest samples.

| Tests | AnnoTest object before | AnnoTest object after | GoogleTest object |
| ---: | ---: | ---: | ---: |
| 1 | 4,262,960 B | 1,835,296 B | 456,440 B |
| 24 | 4,334,944 B | 1,907,192 B | 799,152 B |
| 72 | 4,453,112 B | 2,025,280 B | 1,470,576 B |
| 240 | 4,871,240 B | 2,443,080 B | 3,835,416 B |

Object sizes include Debug information. Every generated executable returned
success and verified that all expected test bodies executed. The after benchmark
objects contain zero `CLI::` symbols.

Exact invocation, after enabling `BUILD_BENCHMARKS` in the existing Debug build:

```sh
python3 benchmarks/run.py \
  --build-dir /workspaces/improved-features/build/container-fixes \
  --sizes 1 24 72 240 --repetitions 3
```

[The saved JSON](results-debug-gcc16.json) contains both snapshots, all timing/RSS
samples, compiler metadata, generated-source hashes, and module hashes including
`include/test.cxx` and `include/runner.cxx` for the after snapshot. Measurements
include uncommitted changes; module hashes identify their precise contents.
Full local build reports additionally retain exact compile commands.

These results describe this regular-test workload and warm Debug compilation.
They make no claim about clean builds, Release compilation, parameterized tests,
or runtime performance. See [README.md](README.md) for methodology, limitations,
and reproducible configuration commands.
