from pathlib import Path
import tempfile
import xml.etree.ElementTree as ET

from support import RunnerTest


class ReportingTests(RunnerTest):
    def test_complete_failure_report(self):
        _, root, case = self.single_case("BehaviorTests", "explicit_source_location", 1)
        # Only time, timestamp, absolute source prefix, and stack frames vary.
        attributes = dict(root.attrib)
        self.assertTrue(attributes.pop("file").replace("\\", "/").endswith("/tests/behavior_tests.cpp"))
        attributes.pop("time")
        attributes.pop("timestamp")
        self.assertEqual(attributes, {"name": "BehaviorTests", "tests": "1",
                                     "failures": "1", "skipped": "0", "errors": "0"})
        attributes = dict(case.attrib)
        attributes.pop("time")
        self.assertEqual(attributes, {"classname": "BehaviorTests",
                                     "name": "explicit_source_location", "errors": "0"})
        self.assertEqual([child.tag for child in case],
                         ["system-out", "system-err", "failure", "failure"])
        self.assertEqual(case[0].attrib, {})
        self.assertEqual(case[1].attrib, {})
        self.assertFalse(case[0].text)
        self.assertFalse(case[1].text)
        for failure, kind, message in zip(case.findall("failure"),
                                         ("expectation", "assertion"),
                                         ("Expectation failed: expected 17, got 29",
                                          "Assertion failed: expected 31, got 43")):
            self.assertEqual(failure.attrib, {"type": kind, "message": message})
            self.assertEqual(failure.text.split("Stacktrace:")[0],
                             f"Location: explicit_location.cpp:400\nMessage: {message}\n")
            self.assertEqual(list(failure), [])

    def test_stdout_stderr_and_xml_escaping(self):
        result, root, case = self.single_case("LifecycleTests", "output")
        self.assertEqual(case.findtext("system-out"), "setup 1\ncout < & ]]>\nprintf\ncleanup 1\n")
        self.assertEqual(case.findtext("system-err"), "cerr < & ]]>\nfprintf\n")
        self.assertNotIn("cout <", result.stdout)
        self.assertNotIn("cerr <", result.stderr)
        self.assertTrue(root.attrib["file"].endswith("lifecycle_tests.cpp"))

    def test_large_capture_does_not_deadlock_or_truncate(self):
        _, _, case = self.single_case("LifecycleTests", "large_output")
        self.assertEqual(case.findtext("system-out"), "setup 1\n" + "o" * 200000 + "cleanup 1\n")
        self.assertEqual(case.findtext("system-err"), "e" * 200000)

    def test_exception_message_round_trips_special_characters(self):
        _, _, case = self.single_case("LifecycleTests", "exception_fail", 1)
        failure = self.failure(case, "uncaught_exception")[0]
        self.assertEqual(failure.attrib["message"], 'test: exception < & " ]]> message')
        self.assertIn('exception < & " ]]> message', failure.text)

    def test_report_totals_count_tests_instead_of_individual_failures(self):
        _, root = self.report("ExpectationTests", ["ExpectationTests.multiple_fail", "ExpectationTests.equal_pass"], 1)
        self.assertEqual(root.attrib["tests"], "2")
        self.assertEqual(root.attrib["failures"], "1")
        self.assertEqual(len(root.findall("testcase/failure")), 3)
        self.assertEqual(root.attrib["errors"], "0")

    def test_capture_does_not_leak_between_cases(self):
        _, root = self.report("LifecycleTests", ["LifecycleTests.output", "LifecycleTests.last"])
        cases = root.findall("testcase")
        self.assertEqual(cases[1].findtext("system-out"), "setup 2\nlast 2\ncleanup 2\n")
        self.assertFalse(cases[1].findtext("system-err"))

    def test_xml_output_attached_path_spaces_and_overwrite(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "report with spaces café.xml"
            path.write_text("stale data" * 1000)
            result = self.run_binary("AnnotationTests", ["--test-name=AnnotationTests.defaults", f"--xml-output={path}"])
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(ET.parse(path).getroot().attrib["tests"], "1")
            self.assertNotIn("stale data", path.read_text())

    def test_unwritable_or_missing_report_destination(self):
        with tempfile.TemporaryDirectory() as directory:
            for path in (directory, str(Path(directory) / "missing" / "report.xml")):
                with self.subTest(path=path):
                    result = self.run_binary("AnnotationTests", ["--test-name", "AnnotationTests.defaults", "--xml-output", path])
                    self.assertEqual(result.returncode, 1)
                    self.assertIn("Cannot write XML report", result.stderr)
                    self.assertIn("defaults: Passed", result.stdout)

    def test_list_and_help_do_not_write_report(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "report.xml"
            for option in ("--list", "--help"):
                result = self.run_binary("AnnotationTests", [option, "--xml-output", str(path)])
                self.assertEqual(result.returncode, 0)
                self.assertFalse(path.exists())

    def test_repeated_runs_reset_suite_log(self):
        result, root, case = self.single_case("LifecycleTests", "passing", scenario="repeat")
        self.assertEqual(result.stdout.count("Running specified tests"), 2)
        self.assertEqual(root.attrib["tests"], "1")
        self.assertEqual(root.attrib["failures"], "0")
        self.assertEqual(root.attrib["errors"], "0")
        self.assertEqual(case.findtext("system-out"), "setup 1\nbody 1\ncleanup 1\n")
