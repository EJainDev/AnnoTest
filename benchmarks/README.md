# Benchmarks

Compare AnnoTest and GoogleTest using matching regular, value-parameterized, and
output-heavy tests at 1, 24, 72, and 240 cases. Recorded compile times are in the
[project README](../README.md#compilation-benchmarks) and
[performance documentation](../docs/source/performance.rst).

Run from the repository root in the development container, without concurrent builds:

```sh
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
```

Warm compilation measures the test source file with dependencies already built,
excluding linking and execution. Samples alternate frameworks and bypass compiler
caches. The checker requires AnnoTest's median to match or beat GoogleTest in
every comparison and verifies source hashes. Reports stay in the build directory.

Add `--cold` to measure clean target builds. With installed GoogleTest, its binary
stays prebuilt while AnnoTest and its modules rebuild from source. Runtime and
peak memory are also recorded; AnnoTest captures output, while GoogleTest writes
it directly.

The [C++ workloads](workloads/), [generator](generate.py),
[measurement harness](run.py), and [checker](check_results.py) are included here.
