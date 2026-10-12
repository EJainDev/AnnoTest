"""Measure warm/clean compilation and runtime using an existing Ninja benchmark build."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import shlex
import statistics
import subprocess
import tempfile
import time


def run(command, cwd, quiet=False):
    subprocess.run(command, cwd=cwd, check=True,
                   stdout=subprocess.DEVNULL if quiet else None)


def measure(command, cwd):
    # wait4 returns this invocation's resource usage, unlike cumulative RUSAGE_CHILDREN.
    start = time.perf_counter()
    with subprocess.Popen(command, cwd=cwd, stdout=subprocess.DEVNULL) as process:
        _, status, usage = os.wait4(process.pid, 0)
        process.returncode = os.waitstatus_to_exitcode(status)
        if process.returncode:
            raise subprocess.CalledProcessError(process.returncode, command)
    return {"wall_seconds": time.perf_counter() - start,
            "peak_rss_kib": usage.ru_maxrss}


def compile_command(entry, output):
    command = entry.get("arguments") or shlex.split(entry["command"])
    result = []
    index = 0
    while index < len(command):
        item = command[index]
        if item in ("-o", "-MF", "-MT", "-MQ"):
            index += 2
            continue
        if item in ("-MD", "-MMD", "-MP") or item.startswith("-fdeps-format="):
            index += 1
            continue
        result.append(item)
        index += 1
    # Bypass compiler launchers: cache hits are not compiler measurements.
    if Path(result[0]).name in ("ccache", "sccache"):
        result.pop(0)
    return [*result, "-o", str(output)]


def summarize(samples):
    return {"samples": samples,
            "median_wall_seconds": statistics.median(s["wall_seconds"] for s in samples),
            "median_peak_rss_kib": statistics.median(s["peak_rss_kib"] for s in samples)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--sizes", type=int, nargs="+", default=[1, 24, 72, 240])
    parser.add_argument("--repetitions", type=int, default=5)
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--workloads", nargs="+", choices=("regular", "parameterized", "output"),
                        default=["regular", "parameterized", "output"])
    parser.add_argument("--runtime-repetitions", type=int, default=5)
    parser.add_argument("--cold", action="store_true", help="Also time clean target builds including dependencies and linking")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    if args.runtime_repetitions < 1 or args.repetitions < 1 or args.jobs < 1 or any(n < 1 for n in args.sizes):
        parser.error("repetitions, jobs and sizes must be positive")
    build = args.build_dir.resolve()
    output = args.output or build / "benchmarks/results.json"
    metadata = json.loads((build / "benchmarks/metadata.json").read_text())
    source = Path(__file__).resolve().parents[1]
    for key, arguments in (("annotest_git_head", ["rev-parse", "HEAD"]),
                           ("annotest_git_status", ["status", "--short"])):
        git = subprocess.run(["git", *arguments], cwd=source, text=True,
                             stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        metadata[key] = git.stdout.strip() if git.returncode == 0 else None
    metadata["annotest_module_sha256"] = {
        str(path.relative_to(source)): hashlib.sha256(path.read_bytes()).hexdigest()
        for path in sorted((source / "include").iterdir()) if path.is_file()}

    if platform.system() != "Linux" or not hasattr(os, "wait4"):
        parser.error("Linux with os.wait4 is required for per-invocation RSS measurements")
    results = {"metadata": {**metadata, "utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
                            "platform": platform.platform(), "cpu_count": os.cpu_count(),
                            "repetitions": args.repetitions, "build_jobs": args.jobs,
                            "runtime_repetitions": args.runtime_repetitions,
                            "runtime_scope": "whole executable; startup, discovery, execution, console, shutdown",
                            "warm_scope": "one test translation unit; dependencies prebuilt; no linking or cache",
                            "cold_scope": "clean target build; pre-fetched dependencies; compilation and linking"},
               "workloads": []}
    for workload in args.workloads:
        for count in args.sizes:
            record = {"test_count": count, "workload": workload, "frameworks": {}}
            for framework in ("annotest", "gtest"):
                stem = framework if workload == "regular" else f"{framework}_{workload}"
                target = f"{stem}_bench_{count}"
                print(f"Preparing {target}", flush=True)
                run(["cmake", "--build", str(build), "--target", target, "-j", str(args.jobs)], build)
                # The generated main verifies every expected test body executed.
                run([str(build / "benchmarks" / target)], build, quiet=True)
                database = json.loads((build / "compile_commands.json").read_text())
                entries = [e for e in database if Path(e["file"]).name == f"{stem}_{count}.cpp"]
                if len(entries) != 1:
                    parser.error(f"Expected one compile command for {target}, got {len(entries)}")
                entry = entries[0]
                with tempfile.TemporaryDirectory(prefix="annotest-benchmark-") as scratch:
                    obj = Path(scratch) / "benchmark.o"
                    command = compile_command(entry, obj)
                    print(f"Measuring {target}: {args.repetitions} compilations", flush=True)
                    samples = [measure(command, entry["directory"]) for _ in range(args.repetitions)]
                    result = {"warm_compile": summarize(samples), "object_bytes": obj.stat().st_size,
                              "command": command[:-1] + ["<temporary-object>"],
                              "source_sha256": hashlib.sha256(Path(entry["file"]).read_bytes()).hexdigest()}
                    result["runtime"] = summarize([
                        measure([str(build / "benchmarks" / target)], build)
                        for _ in range(args.runtime_repetitions)])
                    result["payload_bytes"] = count * 1024 * 1024 if workload == "output" else 0
                    result["output_capture"] = framework == "annotest"
                    record["frameworks"][framework] = result
            anno = record["frameworks"]["annotest"]["warm_compile"]["median_wall_seconds"]
            gtest = record["frameworks"]["gtest"]["warm_compile"]["median_wall_seconds"]
            record["annotest_over_gtest_compile_ratio"] = anno / gtest
            print(f"{count} tests: AnnoTest {anno:.3f}s; GoogleTest {gtest:.3f}s; ratio {anno / gtest:.2f}x", flush=True)
            results["workloads"].append(record)
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_text(json.dumps(results, indent=2) + "\n")
    if args.cold:
        for record in results["workloads"]:
            for framework, result in record["frameworks"].items():
                stem = framework if record["workload"] == "regular" else f"{framework}_{record['workload']}"
                target = f"{stem}_bench_{record['test_count']}"
                print(f"Measuring clean builds of {target}", flush=True)
                command = ["cmake", "--build", str(build), "--target", target, "--clean-first", "-j", str(args.jobs)]
                result["clean_build"] = summarize([measure(command, build) for _ in range(args.repetitions)])
                run([str(build / "benchmarks" / target)], build, quiet=True)
        output.write_text(json.dumps(results, indent=2) + "\n")
    print(f"Results: {output}")


if __name__ == "__main__":
    main()
