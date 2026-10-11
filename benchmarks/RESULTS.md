# Recorded compile benchmark

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
