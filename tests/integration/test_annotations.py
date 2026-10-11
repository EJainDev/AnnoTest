from support import RunnerTest


class AnnotationTests(RunnerTest):
    def test_names_defaults_static_and_const(self):
        for name in ("implicit_name", "custom name", "defaults", "static_method", "const_method"):
            with self.subTest(name=name):
                self.single_case("AnnotationTests", name)
        _, names = self.listed_names("AnnotationTests")
        for absent in ("original_name", "unannotated", "os_without_test", "inaccessible"):
            self.assertNotIn(absent, names)
        self.assertIn("custom name", names)
        self.assertIn("disabled", names)

    def test_unrelated_method_annotations_are_ignored(self):
        for name, output in (("unrelated_before_test", "unrelated before\n"),
                             ("unrelated_after_test", "unrelated after\n")):
            with self.subTest(name=name):
                _, _, case = self.single_case("AnnotationTests", name)
                self.assertEqual(case.findtext("system-out"), output)
                self.assertEqual(case.findall("failure"), [])
        _, names = self.listed_names("AnnotationTests")
        self.assertNotIn("unrelated_without_test", names)

    def test_os_annotations_and_disabled_tests(self):
        for name in ("required_current", "required_multiple", "disallowed_other"):
            self.single_case("AnnotationTests", name)
        for name, count in (("disabled", 1), ("required_other", 1), ("disallowed_current", 1),
                            ("disallowed_multiple", 1), ("contradictory_os", 1),
                            ("os_values", 2), ("required_values", 2), ("os_values.2", 1)):
            with self.subTest(name=name):
                _, root = self.report("AnnotationTests", [f"AnnotationTests.{name}"])
                self.assertEqual(root.attrib["skipped"], str(count))
                self.assertEqual(root.findall("testcase"), [])

    def test_empty_suite(self):
        self.assertEqual(self.listed_names("AnnotationTests", "empty"), ("EmptySuite", []))
        _, root = self.report("AnnotationTests", scenario="empty")
        self.assertEqual(root.attrib["tests"], "0")

    def test_suite_name_annotation(self):
        self.assertEqual(self.listed_names("AnnotationTests", "named"), ("renamed suite", ["body"]))
        _, root = self.report("AnnotationTests", ["renamed suite.body"], scenario="named")
        self.assertEqual(root.attrib["name"], "renamed suite")
        self.assertEqual(root.find("testcase").attrib["classname"], "renamed suite")

    def test_disabled_suite_annotation(self):
        _, root = self.report("AnnotationTests", scenario="disabled")
        self.assertEqual(root.attrib["tests"], "3")
        self.assertEqual(root.attrib["skipped"], "3")
        self.assertEqual(root.findall("testcase"), [])
        _, root = self.report("AnnotationTests", ["DisabledSuite.values.2"], scenario="disabled")
        self.assertEqual(root.attrib["skipped"], "1")

    def test_multiple_selected_disabled_instances(self):
        _, root = self.report("AnnotationTests", ["DisabledSuite.values.1", "DisabledSuite.values.2"],
                              scenario="disabled")
        self.assertEqual(root.attrib["skipped"], "2")
        self.assertEqual(root.findall("testcase"), [])

    def test_selected_os_skips_count_only_requested_instances(self):
        for name in ("required_values", "os_values"):
            for indices in (("2",), ("2", "1")):
                with self.subTest(name=name, indices=indices):
                    _, root = self.report("AnnotationTests", [f"AnnotationTests.{name}.{i}" for i in indices])
                    self.assertEqual(root.attrib["skipped"], str(len(indices)))
                    self.assertEqual(root.findall("testcase"), [])
