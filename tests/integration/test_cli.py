from support import RunnerTest


class CliTests(RunnerTest):
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
