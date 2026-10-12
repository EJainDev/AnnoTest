from support import RunnerTest


class ParameterizationTests(RunnerTest):
    def test_all_parameterization_forms(self):
        groups = {
            "values": ["value 1", "value 2", "value 3"],
            "multiple_arguments": [None, None],
            "cross_values": ["cross 5", "cross 7"],
            "tuple_values": ["tuple 8", "tuple 9"],
            "unequal_value_tuples": ["unequal 8", "unequal 9", "unequal 12"],
            "empty_value_tuples": [None],
            "multiple_value_tuples": ["value tuple 8", "value tuple 9", "value tuple 10", "value tuple 11"],
            "multiple_dual_tuples": ["dual tuple 7", "dual tuple 2.5", "dual tuple 2.5", "dual tuple 7"],
            "no_arguments": ["empty arguments", "empty arguments"],
            "types": ["type int", "type double"],
            "cross_types": ["cross type char", "cross type double"],
            "dual": ["dual 7", "dual 2.5"],
            "tuple_dual": ["tuple dual 7", "tuple dual 2.5"],
            "named values": ["named 4", "named 5"],
        }
        for name, messages in groups.items():
            with self.subTest(name=name):
                _, root = self.report("ParameterizationTests", [f"ParameterizationTests.{name}"])
                cases = root.findall("testcase")
                self.assertEqual([c.attrib["name"] for c in cases],
                                 [f"{name}.{i}" for i in range(1, len(messages) + 1)])
                for i, (case, message) in enumerate(zip(cases, messages), 1):
                    output = case.findtext("system-out", "")
                    self.assertIn("setup 1", output)
                    self.assertIn("cleanup 1", output)
                    if message:
                        self.assertIn(message, output)
                    _, _, selected = self.single_case("ParameterizationTests", f"{name}.{i}")
                    self.assertIn("setup 1", selected.findtext("system-out", ""))
                    if message:
                        self.assertIn(message, selected.findtext("system-out", ""))

    def test_disabled_parameterizations(self):
        _, root = self.report("ParameterizationTests", ["ParameterizationTests.disabled"])
        self.assertEqual(root.attrib["skipped"], "2")
        _, root = self.report("ParameterizationTests", ["ParameterizationTests.disabled.2"])
        self.assertEqual(root.attrib["skipped"], "1")
        self.assertTrue(all(case.find("skipped") is not None for case in root.findall("testcase")))

    def test_filter_deduplicates_and_selects_in_requested_order(self):
        _, root = self.report("ParameterizationTests", ["ParameterizationTests.values.3",
                                                       "ParameterizationTests.values.1",
                                                       "ParameterizationTests.values.3"])
        self.assertEqual([c.attrib["name"] for c in root.findall("testcase")], ["values.3", "values.1"])
        for names in (["values.2", "values"], ["values", "values.2"]):
            _, root = self.report("ParameterizationTests", [f"ParameterizationTests.{n}" for n in names])
            self.assertEqual(len(root.findall("testcase")), 3)

    def test_failure_does_not_stop_later_parameterizations(self):
        _, root = self.report("ParameterizationTests", ["ParameterizationTests.failure_continues"], 1)
        cases = root.findall("testcase")
        self.assertEqual(len(cases), 3)
        self.assertEqual([bool(c.findall("failure")) for c in cases], [False, True, False])
        self.assertIn("cleanup 1", cases[1].findtext("system-out"))
        self.assertIn("value 3", cases[2].findtext("system-out"))

    def test_invalid_indices(self):
        for index in ("0", "-1", "4", "x", "1x", "1.2", "", "99999999999999999999"):
            with self.subTest(index=index):
                result = self.run_binary("ParameterizationTests", ["--test-name", f"ParameterizationTests.values.{index}"])
                self.assertEqual(result.returncode, 1)
                self.assertIn("Invalid parameterization", result.stderr)

    def test_listing(self):
        _, names = self.listed_names("ParameterizationTests")
        self.assertIn("values.1", names)
        self.assertIn("values.3", names)
        self.assertIn("dual.2", names)
        self.assertIn("types.2", names)
        self.assertIn("named values.2", names)
        self.assertIn("disabled.2", names)
        self.assertNotIn("empty", names)
        self.assertNotIn("values", names)
