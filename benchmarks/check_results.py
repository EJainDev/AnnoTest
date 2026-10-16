"""Check the complete release compile-time matrix against GoogleTest.

Use warm matrix reports for Debug and Release. Clean builds can be checked
separately. Medians are recomputed from raw samples; missing data fails the gate.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import statistics


def check(paths, root, sizes=(1, 24, 72, 240), build_types=("Debug", "Release"),
          include_clean=False):
    expected = {(mode, workload, count, "warm_compile")
                for mode in build_types
                for workload in ("regular", "parameterized", "output")
                for count in sizes}
    if include_clean:
        expected.update((mode, "regular", 1, "clean_build") for mode in build_types)
    measurements = {}
    errors = []
    hashes = {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
              for p in sorted((root / "include").iterdir()) if p.is_file()}
    tool_hashes = {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
                   for p in (root / "benchmarks/run.py", root / "benchmarks/generate.py")}
    for path in paths:
        report = json.loads(path.read_text())
        metadata = report["metadata"]
        if metadata["annotest_module_sha256"] != hashes:
            errors.append(f"{path}: source hashes do not match the current library")
        if metadata.get("benchmark_tool_sha256") != tool_hashes:
            errors.append(f"{path}: benchmark tool hashes do not match the current harness and generator")
        for row in report["workloads"]:
            for scope in ("warm_compile", "clean_build"):
                frameworks = row["frameworks"]
                if not any(scope in result for result in frameworks.values()):
                    continue
                key = (metadata["build_type"], row["workload"], row["test_count"], scope)
                if key not in expected:
                    continue
                for framework in ("annotest", "gtest"):
                    stem = framework if row["workload"] == "regular" else framework + "_" + row["workload"]
                    source = root / "benchmarks/workloads" / f"{stem}_{row['test_count']}.cpp"
                    digest = hashlib.sha256(source.read_bytes()).hexdigest() if source.is_file() else None
                    if digest is None or frameworks.get(framework, {}).get("source_sha256") != digest:
                        errors.append(f"{path}: {key} benchmark source does not match {source}")
                medians = {}
                for framework in ("annotest", "gtest"):
                    samples = frameworks.get(framework, {}).get(scope, {}).get("samples", [])
                    values = [sample["wall_seconds"] for sample in samples]
                    if len(values) < 3 or any(not math.isfinite(v) or v <= 0 for v in values):
                        errors.append(f"{path}: {key} requires three positive finite samples for {framework}")
                        break
                    medians[framework] = statistics.median(values)
                if len(medians) != 2:
                    continue
                ratio = medians["annotest"] / medians["gtest"]
                measurements.setdefault(key, []).append(ratio)
                if ratio > 1:
                    errors.append(f"{key}: AnnoTest / GoogleTest = {ratio:.3f}, exceeds 1.000")
    for key in sorted(expected - measurements.keys()):
        errors.append(f"Missing measurement: {key}")
    return errors, measurements


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("reports", nargs="+", type=Path)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--include-clean", action="store_true",
                        help="Also require one-case ordinary clean builds to match GoogleTest")
    args = parser.parse_args()
    errors, measurements = check(args.reports, args.root, include_clean=args.include_clean)
    for key, ratios in sorted(measurements.items()):
        print(f"{key}: {', '.join(f'{ratio:.3f}x' for ratio in ratios)}")
    for error in errors:
        print(error)
    raise SystemExit(bool(errors))


if __name__ == "__main__":
    main()
