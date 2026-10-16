# Recorded performance results

Measured on 2026-10-16 in the development container with GCC 16.2.0, CMake
4.4.3, GNU C++26, and installed GoogleTest 1.15.0. Debug uses `-g` with GCC's
default `-O0`; Release uses `-O3 -DNDEBUG`. All compilation comparisons use
five paired samples, alternating the framework compiled first. Both executables
are built and validated before timing. No concurrent builds ran during measurement.

## Test-file compilation

Times are median seconds for compiling only the test translation unit, with
framework libraries and module interfaces already built. Linking, dependency
scanning, configuration, downloads, and test execution are excluded. Compiler
caches are bypassed. All 24 comparisons passed the release gate; a ratio below
1.0 means AnnoTest compiled faster in that workload.

### Debug

| Workload | Cases | AnnoTest (s) | GoogleTest (s) | AnnoTest / GoogleTest |
| --- | ---: | ---: | ---: | ---: |
| Regular | 1 | 1.739 | 2.158 | 0.806× |
| Regular | 24 | 2.111 | 2.384 | 0.886× |
| Regular | 72 | 2.460 | 3.083 | 0.798× |
| Regular | 240 | 3.489 | 5.079 | 0.687× |
| Value parameters | 1 | 1.880 | 2.766 | 0.680× |
| Value parameters | 24 | 2.412 | 3.337 | 0.723× |
| Value parameters | 72 | 2.489 | 3.388 | 0.735× |
| Value parameters | 240 | 3.000 | 3.613 | 0.830× |
| 1 MiB output per case | 1 | 1.699 | 2.112 | 0.804× |
| 1 MiB output per case | 24 | 2.190 | 2.422 | 0.904× |
| 1 MiB output per case | 72 | 2.637 | 3.178 | 0.830× |
| 1 MiB output per case | 240 | 3.827 | 5.253 | 0.729× |

### Release

| Workload | Cases | AnnoTest (s) | GoogleTest (s) | AnnoTest / GoogleTest |
| --- | ---: | ---: | ---: | ---: |
| Regular | 1 | 1.352 | 2.080 | 0.650× |
| Regular | 24 | 2.350 | 3.100 | 0.758× |
| Regular | 72 | 2.872 | 5.476 | 0.524× |
| Regular | 240 | 4.780 | 14.371 | 0.333× |
| Value parameters | 1 | 1.603 | 3.256 | 0.492× |
| Value parameters | 24 | 2.628 | 4.906 | 0.536× |
| Value parameters | 72 | 2.755 | 4.922 | 0.560× |
| Value parameters | 240 | 3.260 | 5.132 | 0.635× |
| 1 MiB output per case | 1 | 1.399 | 2.074 | 0.674× |
| 1 MiB output per case | 24 | 2.567 | 3.218 | 0.798× |
| 1 MiB output per case | 72 | 3.538 | 5.589 | 0.633× |
| 1 MiB output per case | 240 | 5.886 | 14.559 | 0.404× |

## Clean target builds

Median seconds from three serial clean builds of the one-case regular workload.
Dependencies are already fetched and configured. AnnoTest, its dependencies, and
the standard-library module rebuild from source; the installed GoogleTest binary
remains prebuilt. These measurements include dependency scanning and linking and
are separate from test-file compilation.

| Configuration | AnnoTest (s) | GoogleTest (s) |
| --- | ---: | ---: |
| Debug | 73.304 | 2.363 |
| Release | 103.331 | 2.357 |

[Debug clean-build samples](results-debug-clean.json) and
[Release clean-build samples](results-release-clean.json) contain all three samples.

## Whole-executable runtime

Median of five complete runs, including startup, discovery, fixture operations,
assertions, output handling, and shutdown. Both frameworks use fresh fixtures and
matching case counts and integer checks. Every executable verifies the expected
number of executed test bodies. AnnoTest calls a runner per fixture class;
GoogleTest uses one global runner. The harness discards stdout. AnnoTest captures
and retains test output; GoogleTest writes it directly. The output workload emits
1 MiB per case, or 240 MiB per 240-case run. These measurements include the
frameworks' default output retention costs and do not measure per-assertion
latency or parallel workers. RSS is the median child-process high-water mark and
includes launch overhead.

### Debug

| Workload | Cases | AnnoTest (ms) | GoogleTest (ms) | AnnoTest RSS (MiB) | GoogleTest RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| Regular | 1 | 4.195 | 2.458 | 18.5 | 18.5 |
| Regular | 24 | 5.268 | 2.197 | 18.6 | 18.6 |
| Regular | 72 | 9.531 | 3.039 | 18.7 | 18.7 |
| Regular | 240 | 27.885 | 5.181 | 18.7 | 18.7 |
| Value parameters | 1 | 2.669 | 2.367 | 18.7 | 18.7 |
| Value parameters | 24 | 4.338 | 2.947 | 19.2 | 19.2 |
| Value parameters | 72 | 7.875 | 2.788 | 19.2 | 19.2 |
| Value parameters | 240 | 23.308 | 4.436 | 19.2 | 19.2 |
| 1 MiB output per case | 1 | 7.857 | 2.693 | 19.2 | 19.2 |
| 1 MiB output per case | 24 | 126.181 | 7.008 | 30.1 | 19.2 |
| 1 MiB output per case | 72 | 250.615 | 11.280 | 77.2 | 19.2 |
| 1 MiB output per case | 240 | 716.197 | 15.796 | 245.4 | 19.2 |

### Release

| Workload | Cases | AnnoTest (ms) | GoogleTest (ms) | AnnoTest RSS (MiB) | GoogleTest RSS (MiB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| Regular | 1 | 2.848 | 2.683 | 18.8 | 18.8 |
| Regular | 24 | 3.189 | 3.473 | 18.8 | 18.8 |
| Regular | 72 | 6.755 | 3.026 | 18.8 | 18.8 |
| Regular | 240 | 14.834 | 3.588 | 18.8 | 18.8 |
| Value parameters | 1 | 1.978 | 2.628 | 18.8 | 18.8 |
| Value parameters | 24 | 4.773 | 2.435 | 19.2 | 19.2 |
| Value parameters | 72 | 9.946 | 4.465 | 19.2 | 19.2 |
| Value parameters | 240 | 16.732 | 3.281 | 19.2 | 19.2 |
| 1 MiB output per case | 1 | 9.951 | 4.735 | 19.2 | 19.2 |
| 1 MiB output per case | 24 | 73.554 | 5.524 | 30.1 | 19.2 |
| 1 MiB output per case | 72 | 213.049 | 9.834 | 77.1 | 19.2 |
| 1 MiB output per case | 240 | 696.566 | 27.401 | 245.2 | 19.2 |

## Sources and reproduction

[Debug samples](results-debug-warm.json) and [Release samples](results-release-warm.json)
include every timing and RSS sample, compiler commands, object sizes, metadata,
and library, workload, harness, and generator hashes. The [matching C++ sources](workloads/),
[generator](generate.py), [measurement harness](run.py), and [gate](check_results.py)
are public. See [the benchmark guide](README.md) for reproduction commands and
measurement scope. Results describe these workloads and this configuration.
