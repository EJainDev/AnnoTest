"""Independent black-box checks of the public executable and XML interfaces."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET


class RunnerTest(unittest.TestCase):
    binary_dir = None

    def run_binary(self, executable, args=(), scenario=None):
        env = os.environ.copy()
        env.pop("ANNOTEST_SCENARIO", None)
        if scenario is not None:
            env["ANNOTEST_SCENARIO"] = scenario
        return subprocess.run(
            [str(self.binary_dir / (executable + (".exe" if os.name == "nt" else ""))), *args],
            capture_output=True, text=True, encoding="utf-8", env=env, timeout=20,
        )

    def report(self, executable, names=(), code=0, scenario=None, options=()):
        with tempfile.TemporaryDirectory(prefix="annotest-test-") as directory:
            path = Path(directory) / "report.xml"
            args = ["--xml-output", str(path), *options]
            if names:
                args.extend(["--test-name", *names])
            result = self.run_binary(executable, args, scenario)
            self.assertEqual(result.returncode, code, result.stdout + result.stderr)
            self.assertTrue(path.is_file(), result.stdout + result.stderr)
            root = ET.parse(path).getroot()
        self.assertEqual(root.tag, "testsuite")
        self.assertGreaterEqual(float(root.attrib["time"]), 0)
        self.assertRegex(root.attrib["timestamp"], r"^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\dZ$")
        cases = root.findall("testcase")
        self.assertEqual(int(root.attrib["tests"]), len(cases) + int(root.attrib["skipped"]))
        self.assertEqual(int(root.attrib["failures"]), sum(bool(c.findall("failure")) for c in cases))
        self.assertEqual(int(root.attrib["errors"]), sum(int(c.attrib["errors"]) for c in cases))
        for case in cases:
            self.assertEqual(case.attrib["classname"], root.attrib["name"])
            self.assertGreaterEqual(float(case.attrib["time"]), 0)
        return result, root

    def listed_names(self, executable, scenario=None):
        result = self.run_binary(executable, ["--list"], scenario)
        self.assertEqual(result.returncode, 0, result.stderr)
        lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
        self.assertTrue(lines[0].endswith("."))
        return lines[0][:-1], lines[1:]

    def single_case(self, executable, name, code=0, scenario=None):
        result, root = self.report(executable, [f"{executable}.{name}"], code, scenario)
        cases = root.findall("testcase")
        self.assertEqual(len(cases), 1)
        self.assertEqual(cases[0].attrib["name"], name)
        return result, root, cases[0]

    def failure(self, case, kind, count=1):
        failures = case.findall("failure")
        self.assertEqual(len(failures), count)
        for failure in failures:
            self.assertEqual(failure.attrib["type"], kind)
            self.assertTrue(failure.attrib["message"])
            self.assertIn("Message:", failure.text)
        return failures
