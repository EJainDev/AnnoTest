from support import RunnerTest


class GenericTests(RunnerTest):
    def comparison_case(self, name, index, code, counts, message=None, kind=None):
        _, root, case = self.single_case("GenericTests", f"{name}.{index}", code)
        output = case.findtext("system-out", "")
        self.assertIn(f"calls:{counts}\n", output)
        nonfatal = index % 2 == 0
        self.assertEqual("continued\n" in output, code == 0 or (nonfatal and kind == "expectation"))
        self.assertEqual(root.attrib["errors"], "1" if kind == "uncaught_exception" else "0")
        if kind:
            self.assertEqual(self.failure(case, kind)[0].attrib["message"], message)
        else:
            self.assertEqual(case.findall("failure"), [])

    def test_exceptions_do_not_stop_next_case(self):
        for name in ("throwing_comparison", "throwing_conversion", "throwing_formatter"):
            with self.subTest(name=name):
                _, root = self.report("GenericTests", [f"GenericTests.{name}.2",
                                                       "GenericTests.recovery_after_exception"], 1)
                self.assertEqual(root.attrib["errors"], "1")
                cases = root.findall("testcase")
                self.assertEqual([c.attrib["name"] for c in cases],
                                 [f"{name}.2", "recovery_after_exception"])
                self.assertEqual(cases[1].findall("failure"), [])
                self.assertIn("continued", cases[1].findtext("system-out"))

    def test_custom_containment(self):
        for index in (1, 2):
            self.comparison_case("contains_custom_values", index, 0, "2:2:0")
            kind = "expectation" if index == 2 else "assertion"
            prefix = "Expectation" if index == 2 else "Assertion"
            self.comparison_case("contains_custom_failure", index, 1, "2:2:1",
                                 f"{prefix} failed: container does not contain value Value(11)", kind)


def comparison_test(name, index, code, counts, message=None, kind=None):
    def check(self):
        self.comparison_case(name, index, code, counts, message, kind)
    return check


# Register separately so CTest exposes every operator/helper and failure stage.
for operation, operands in enumerate(((7, 9), (7, 7), (7, 7), (7, 6), (9, 9), (7, 9))):
    for nonfatal in (False, True):
        index = operation * 2 + int(nonfatal) + 1
        kind = "expectation" if nonfatal else "assertion"
        prefix = "Expectation" if nonfatal else "Assertion"
        left, right = operands
        unequal = "not equal to " if operation == 1 else ""
        diagnostic = f"expected {unequal}Value({left}), got Value({right})"
        if operation >= 2:
            operator = ('<', '<=', '>', '>=')[operation - 2]
            diagnostic = f"expected Value({left}) {operator} Value({right}), relation was false"
        scenarios = (
            ("comparisons_pass", 0, "1:1:0", None, None),
            ("comparisons_fail", 1, "1:1:2",
             f"{prefix} failed: {diagnostic}", kind),
            ("throwing_comparison", 1, "1:0:0", "test: comparison threw", "uncaught_exception"),
            ("throwing_conversion", 1, "1:1:0", "test: proxy conversion threw", "uncaught_exception"),
            ("throwing_formatter", 1, "1:1:1", "test: formatter threw", "uncaught_exception"),
        )
        for name, code, counts, message, failure_kind in scenarios:
            setattr(GenericTests, f"test_{name}_{index}",
                    comparison_test(name, index, code, counts, message, failure_kind))

for false_check in (False, True):
    for nonfatal in (False, True):
        index = int(false_check) * 2 + int(nonfatal) + 1
        setattr(GenericTests, f"test_truth_pass_{index}",
                comparison_test("truth_pass", index, 0, "0:1:0"))
        kind = "expectation" if nonfatal else "assertion"
        prefix = "Expectation" if nonfatal else "Assertion"
        expected, actual = ("false", "true") if false_check else ("true", "false")
        setattr(GenericTests, f"test_truth_fail_{index}",
                comparison_test("truth_fail", index, 1, "0:1:1",
                                f"{prefix} failed: expected {expected}, got {actual} for proxy({actual})",
                                kind))
