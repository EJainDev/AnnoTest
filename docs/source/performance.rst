Performance
===========

Test-file compilation
---------------------

The compilation benchmark compares matching AnnoTest and GoogleTest test
translation units in Debug and Release configurations. The matrix contains
1, 24, 72, and 240 cases for each of three workloads:

* Regular tests with fixture setup, cleanup, and an integer equality assertion.
* Value-parameterized tests with the same number of concrete invocations.
* Regular tests that also write 1 MiB of output per invocation.

Each executable verifies that every expected test body executed. Both frameworks
use fresh fixtures and matching checks. Workloads use three fixture classes,
except that the one-case workload uses one.

Warm compilation measures direct compiler invocations for the test source file,
with framework libraries and module interfaces already built. It excludes
linking, dependency scanning, configuration, downloads, and test execution.
Compiler caches are bypassed. Both frameworks use the same compiler, C++26
language mode, and build-type flags; AnnoTest also uses its required reflection
and module options. Neither target uses unity builds or a precompiled header.
Both executables are built and validated before measurement. Compiler samples
are paired, with the first framework alternating between pairs and workloads;
reports record the sample order.

The release gate checks every workload and case count in both configurations.
It recomputes medians from at least three positive, finite raw samples, verifies
the library, workload, harness, and generator source hashes, and requires an AnnoTest / GoogleTest
ratio no greater than 1.0 for every comparison. Performance depends on the
compiler, hardware, build settings, and test contents; these workloads do not
establish compilation times for every application.

Recorded compilation times
--------------------------

Measured on 2026-10-16 with GCC 16.2.0, CMake 4.4.3, GNU C++26, and installed
GoogleTest 1.15.0 in the development container. Debug uses ``-g`` and default
``-O0``; Release uses ``-O3 -DNDEBUG``. All 24 comparisons passed. Ratios are
AnnoTest / GoogleTest; below 1.0 means faster compilation in that workload.

Debug
~~~~~

.. list-table:: Median test-file compilation, seconds (five samples)
   :header-rows: 1

   * - Workload
     - Cases
     - AnnoTest (s)
     - GoogleTest (s)
     - Ratio
   * - Regular
     - 1
     - 1.739
     - 2.158
     - 0.806
   * - Regular
     - 24
     - 2.111
     - 2.384
     - 0.886
   * - Regular
     - 72
     - 2.460
     - 3.083
     - 0.798
   * - Regular
     - 240
     - 3.489
     - 5.079
     - 0.687
   * - Value parameters
     - 1
     - 1.880
     - 2.766
     - 0.680
   * - Value parameters
     - 24
     - 2.412
     - 3.337
     - 0.723
   * - Value parameters
     - 72
     - 2.489
     - 3.388
     - 0.735
   * - Value parameters
     - 240
     - 3.000
     - 3.613
     - 0.830
   * - 1 MiB output per case
     - 1
     - 1.699
     - 2.112
     - 0.804
   * - 1 MiB output per case
     - 24
     - 2.190
     - 2.422
     - 0.904
   * - 1 MiB output per case
     - 72
     - 2.637
     - 3.178
     - 0.830
   * - 1 MiB output per case
     - 240
     - 3.827
     - 5.253
     - 0.729

Release
~~~~~~~

.. list-table:: Median test-file compilation, seconds (five samples)
   :header-rows: 1

   * - Workload
     - Cases
     - AnnoTest (s)
     - GoogleTest (s)
     - Ratio
   * - Regular
     - 1
     - 1.352
     - 2.080
     - 0.650
   * - Regular
     - 24
     - 2.350
     - 3.100
     - 0.758
   * - Regular
     - 72
     - 2.872
     - 5.476
     - 0.524
   * - Regular
     - 240
     - 4.780
     - 14.371
     - 0.333
   * - Value parameters
     - 1
     - 1.603
     - 3.256
     - 0.492
   * - Value parameters
     - 24
     - 2.628
     - 4.906
     - 0.536
   * - Value parameters
     - 72
     - 2.755
     - 4.922
     - 0.560
   * - Value parameters
     - 240
     - 3.260
     - 5.132
     - 0.635
   * - 1 MiB output per case
     - 1
     - 1.399
     - 2.074
     - 0.674
   * - 1 MiB output per case
     - 24
     - 2.567
     - 3.218
     - 0.798
   * - 1 MiB output per case
     - 72
     - 3.538
     - 5.589
     - 0.633
   * - 1 MiB output per case
     - 240
     - 5.886
     - 14.559
     - 0.404

Download :download:`Debug raw samples <../../benchmarks/results-debug-warm.json>`
and :download:`Release raw samples <../../benchmarks/results-release-warm.json>`.
Reports include all compilation and whole-executable runtime samples, RSS,
object sizes, exact commands, and source hashes.

Reproduce the measurements
--------------------------

Run the benchmark on an idle machine, without concurrent builds:

.. code-block:: sh

   for mode in Debug Release; do
     cmake -S . -B "build/benchmark-$mode" -G Ninja \
       -DCMAKE_BUILD_TYPE="$mode" -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF \
       -DBUILD_BENCHMARKS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
       -DCMAKE_CXX_COMPILER_LAUNCHER=
     python3 benchmarks/run.py --build-dir "build/benchmark-$mode" \
       --sizes 1 24 72 240 --repetitions 5 --runtime-repetitions 5 \
       --output "build/benchmark-$mode.json"
   done
   python3 benchmarks/check_results.py \
     build/benchmark-Debug.json build/benchmark-Release.json

Clean builds and runtime
------------------------

Add ``--cold`` to measure clean target builds, including framework compilation,
dependency scanning, and linking. Dependencies are already fetched and
configured. With the installed GoogleTest package, GoogleTest's binary remains
prebuilt; AnnoTest and its dependencies, including the standard-library module,
rebuild from source. Clean builds are recorded separately from the compilation
of user test files.

Recorded medians for one regular case, from three serial clean builds:

.. list-table:: Clean target build, seconds
   :header-rows: 1

   * - Configuration
     - AnnoTest (s)
     - GoogleTest (s)
   * - Debug
     - 73.304
     - 2.363
   * - Release
     - 103.331
     - 2.357

Download :download:`Debug clean-build samples <../../benchmarks/results-debug-clean.json>`
and :download:`Release clean-build samples <../../benchmarks/results-release-clean.json>`.

To record the one-case regular clean-build comparison for both configurations:

.. code-block:: sh

   for mode in Debug Release; do
     python3 benchmarks/run.py --build-dir "build/benchmark-$mode" \
       --sizes 1 --workloads regular --repetitions 3 --runtime-repetitions 5 \
       --cold --output "build/benchmark-$mode-clean.json"
   done

The harness also records whole-executable runtime, including startup, discovery,
fixtures, assertions, output handling, and shutdown. AnnoTest invokes a runner
per fixture class; GoogleTest uses one global runner. AnnoTest captures and
retains output, while GoogleTest's default runner writes it directly. The
harness discards stdout. Runtime samples measure these default behaviors and
their output retention costs; they do not measure per-assertion latency or
parallel workers. Process peak RSS includes launch overhead.

The `full recorded results
<https://github.com/EJainDev/AnnoTest/blob/improved-features/benchmarks/RESULTS.md>`_
include compilation, runtime, and clean-build tables.

The `benchmark sources, generator, measurement harness, and gate
<https://github.com/EJainDev/AnnoTest/tree/improved-features/benchmarks>`_ are
available in the repository. JSON reports include all wall-time and RSS samples,
object sizes, exact commands, toolchain metadata, and source hashes.
