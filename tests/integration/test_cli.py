import json
from pathlib import Path
import tempfile

from support import RunnerTest


class CliTests(RunnerTest):
    def test_dotted_names_belong_to_exact_tests(self):
        for name in ("value.special", "value.special.99", "nested.value.note", "nested.value.2"):
            with self.subTest(name=name):
                _, _, case = self.single_case("NameTests", name)
                self.assertEqual(case.findall("failure"), [])
        _, root = self.report("NameTests", ["NameTests.value.special", "NameTests.value.1"])
        self.assertEqual([case.attrib['name'] for case in root.findall('testcase')],
                         ["value.1", "value.special"])

    def test_leading_hyphen_suite_name(self):
        self.assertEqual(self.listed_names("NameTests", "leading"), ("-smoke", ["body"]))
        _, root = self.report("NameTests", scenario="leading", options=("--test-name=-smoke.body",))
        self.assertEqual([case.attrib['name'] for case in root.findall('testcase')], ["body"])

    def test_help_aliases_and_groups(self):
        for option in ("--help", "-h", "-lh", "-hl"):
            with self.subTest(option=option):
                result = self.run_binary("AnnotationTests", [option])
                self.assertEqual(result.returncode, 0)
                self.assertIn("Usage:", result.stdout)
                self.assertIn("--xml-output", result.stdout)
                self.assertNotIn("Running", result.stdout)
        for options in (("--list",), ("-l",), ("-ltAnnotationTests.defaults",)):
            result = self.run_binary("AnnotationTests", options)
            self.assertEqual(result.returncode, 0)
            self.assertIn("AnnotationTests.", result.stdout)
            self.assertNotIn("Passed", result.stdout)

    def test_filter_aliases_attached_and_repeated_values(self):
        for args in (("--test-name=AnnotationTests.defaults",), ("-tAnnotationTests.defaults",),
                     ("-t=AnnotationTests.defaults",), ("-t", "AnnotationTests.defaults"),
                     ("--test-name", "AnnotationTests.defaults", "AnnotationTests.const_method"),
                     ("--test-name", "AnnotationTests.defaults", "--test-name=AnnotationTests.const_method")):
            with self.subTest(args=args):
                _, root = self.report("AnnotationTests", options=args)
                expected = 2 if any("const_method" in arg for arg in args) else 1
                self.assertEqual(len(root.findall("testcase")), expected)
                self.assertEqual(root.find("testcase").attrib["name"], "defaults")

    def test_invalid_options(self):
        for args in (("--test-name",), ("-t",), ("--xml-output",), ("--unknown",), ("-z",),
                     ("--test-name=",), ("-t=",), ("--xml-output=",), ("--list=true",),
                     ("--help=yes",), ("--", "unexpected"), ("positional",),
                     ("--xml-output", "one.xml", "--xml-output", "two.xml"),
                     ("--test-name", ""), ("--xml-output", ""),
                     ("--xml-output", "--list"), ("--test-name", "--list"),
                     ("--help", "--unknown"), ("-hzl",)):
            with self.subTest(args=args):
                result = self.run_binary("AnnotationTests", args)
                self.assertEqual(result.returncode, 1)
                self.assertIn("Error:", result.stderr)
                self.assertIn("--help", result.stderr)
                self.assertNotIn("Passed", result.stdout)
                self.assertNotIn("Usage:", result.stdout)

    def test_unknown_name_and_unqualified_name(self):
        for name in ("AnnotationTests.missing", "defaults", "OtherSuite.defaults", "AnnotationTests.original_name"):
            with self.subTest(name=name):
                result, root = self.report("AnnotationTests", [name], 1)
                self.assertEqual(root.attrib["tests"], "0")
                self.assertIn("not found", result.stderr)
        result, root = self.report("AnnotationTests", ["AnnotationTests.defaults", "AnnotationTests.missing"], 1)
        self.assertEqual(len(root.findall("testcase")), 1)
        self.assertIn("AnnotationTests.missing", result.stderr)

    def test_duplicate_nonparameterized_name_runs_once(self):
        _, root = self.report("AnnotationTests", ["AnnotationTests.defaults", "AnnotationTests.defaults"])
        self.assertEqual(len(root.findall("testcase")), 1)

    def test_terminator(self):
        result = self.run_binary("UtilityTests", ["--"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("enums: Passed", result.stdout)

    def test_numeric_options_reject_noncanonical_and_out_of_range_values(self):
        values = ("", " ", " 1", "1 ", "+1", "-1", "1.0", "1e2", "0x10",
                  "1_000", "١", "4294967296", "9" * 100)
        for option in ("--repeat", "--jobs", "--seed", "--shard-count", "--shard-index"):
            for value in values:
                with self.subTest(option=option, value=value):
                    result = self.run_binary("AnnotationTests", [f"{option}={value}", "--list"])
                    self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                    self.assertIn("Error:", result.stderr)
                    self.assertNotIn("AnnotationTests.", result.stdout)

    def test_numeric_option_boundaries_are_accepted_without_executing(self):
        # Listing validates the maximum counts without launching workers or repetitions.
        for options in (("--repeat=1",), ("--repeat=1000000",),
                        ("--jobs=1",), ("--jobs=256",),
                        ("--seed=0",), ("--seed=4294967295",),
                        ("--shard-count=1", "--shard-index=0"),
                        ("--shard-count=4294967295", "--shard-index=4294967294"),
                        ("--timeout=0.000001",), ("--timeout=1e3",)):
            with self.subTest(options=options):
                result = self.run_binary("AnnotationTests", [*options, "--list-json"])
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertTrue(json.loads(result.stdout)["suites"])

    def test_boolean_options_only_accept_zero_or_one(self):
        for option in ("--run-disabled", "--shuffle", "--fail-fast", "--isolate",
                       "--brief", "--break-on-failure"):
            for value in ("", "2", "-1", "true", "false", "yes", "01", " 1", "1 "):
                with self.subTest(option=option, value=value):
                    result = self.run_binary("AnnotationTests", [f"{option}={value}", "--list"])
                    self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                    self.assertIn("accepts only 0 or 1", result.stderr)
            for value in ("0", "1"):
                with self.subTest(option=option, value=value):
                    result = self.run_binary("AnnotationTests", [f"{option}={value}", "--list-json"])
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertTrue(json.loads(result.stdout)["suites"])

    def test_timeout_rejects_nonfinite_underflow_and_trailing_input(self):
        for value in ("nan", "NaN", "inf", "-inf", "1e309", "1e-9999", "0", "-0",
                      "-0.1", "+1", " 1", "1 ", "1s", "1.2.3", ""):
            with self.subTest(value=value):
                result = self.run_binary("AnnotationTests", [f"--timeout={value}", "--list"])
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("Error:", result.stderr)

    def test_invalid_cli_does_not_overwrite_existing_reports(self):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / "result.xml"
            report = Path(directory) / "result.json"
            for options in (("--repeat=0",), ("--jobs=257",), ("--shard-count=2", "--shard-index=2"),
                            ("--help", "--unknown"), ("--list-json=1",)):
                with self.subTest(options=options):
                    xml.write_text("existing XML")
                    report.write_text("existing JSON")
                    result = self.run_binary("AnnotationTests", ["--xml-output", str(xml),
                                             "--json-output", str(report), *options])
                    self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                    self.assertEqual(xml.read_text(), "existing XML")
                    self.assertEqual(report.read_text(), "existing JSON")
