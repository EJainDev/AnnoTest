"""Verify that incomplete, stale, or slower benchmark data fails the release gate."""
import copy
import hashlib
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("check_results", ROOT / "benchmarks/check_results.py")
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)


class BenchmarkGateTests(unittest.TestCase):
    def setUp(self):
        def measurement(value):
            return {"samples": [{"wall_seconds": value} for _ in range(3)],
                    "median_wall_seconds": 0.01}
        self.report = {
            "metadata": {"build_type": "Release", "annotest_module_sha256": {
                str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in sorted((ROOT / "include").iterdir()) if p.is_file()}},
            "workloads": [{"workload": workload, "test_count": 1, "frameworks": {
                name: {"warm_compile": measurement(value), "clean_build": measurement(value)}
                for name, value in (("annotest", 1), ("gtest", 2))}}
                for workload in ("regular", "parameterized", "output")]}
        for row in self.report["workloads"]:
            for name, framework in row["frameworks"].items():
                stem = name if row["workload"] == "regular" else name + "_" + row["workload"]
                framework["source_sha256"] = hashlib.sha256(
                    (ROOT / "benchmarks/workloads" / f"{stem}_1.cpp").read_bytes()).hexdigest()
        self.report["metadata"]["benchmark_tool_sha256"] = {
            str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
            for p in (ROOT / "benchmarks/run.py", ROOT / "benchmarks/generate.py")}

    def errors(self, report=None, include_clean=False):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "report.json"
            path.write_text(json.dumps(self.report if report is None else report))
            return gate.check([path], ROOT, sizes=(1,), build_types=("Release",),
                              include_clean=include_clean)[0]

    def test_complete_faster_matrix_passes(self):
        self.assertEqual(self.errors(), [])

    def test_equal_medians_pass(self):
        for row in self.report["workloads"]:
            for scope in ("warm_compile", "clean_build"):
                row["frameworks"]["annotest"][scope] = copy.deepcopy(row["frameworks"]["gtest"][scope])
        self.assertEqual(self.errors(), [])

    def test_missing_workload_and_clean_build_fail(self):
        self.report["workloads"].pop()
        for framework in self.report["workloads"][0]["frameworks"].values():
            del framework["clean_build"]
        errors = self.errors(include_clean=True)
        self.assertTrue(any("output" in error for error in errors))
        self.assertTrue(any("clean_build" in error for error in errors))

    def test_warm_gate_does_not_require_clean_builds(self):
        for row in self.report["workloads"]:
            for framework in row["frameworks"].values():
                del framework["clean_build"]
        self.assertEqual(self.errors(), [])

    def test_stale_source_fails(self):
        self.report["metadata"]["annotest_module_sha256"] = {}
        self.assertTrue(any("source hashes" in error for error in self.errors()))

    def test_stale_workload_source_fails(self):
        self.report["workloads"][0]["frameworks"]["annotest"]["source_sha256"] = "stale"
        self.assertTrue(any("benchmark source" in error for error in self.errors()))

    def test_stale_benchmark_tools_fail(self):
        self.report["metadata"]["benchmark_tool_sha256"] = {}
        self.assertTrue(any("benchmark tool hashes" in error for error in self.errors()))

    def test_slow_warm_and_clean_samples_fail_despite_summary(self):
        for scope in ("warm_compile", "clean_build"):
            with self.subTest(scope=scope):
                report = copy.deepcopy(self.report)
                samples = report["workloads"][0]["frameworks"]["annotest"][scope]["samples"]
                for sample in samples:
                    sample["wall_seconds"] = 3
                self.assertTrue(any("exceeds" in error for error in
                                    self.errors(report, include_clean=scope == "clean_build")))

    def test_invalid_and_insufficient_samples_fail(self):
        for value in (0, -1, float("nan"), float("inf")):
            with self.subTest(value=value):
                report = copy.deepcopy(self.report)
                report["workloads"][0]["frameworks"]["gtest"]["warm_compile"]["samples"][0]["wall_seconds"] = value
                self.assertTrue(any("positive finite" in error for error in self.errors(report)))
        self.report["workloads"][0]["frameworks"]["annotest"]["warm_compile"]["samples"].pop()
        self.assertTrue(any("three positive" in error for error in self.errors()))


if __name__ == "__main__":
    unittest.main()
