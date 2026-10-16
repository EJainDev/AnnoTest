from support import RunnerTest

CHECKS = (
    "equal", "not_equal", "true", "false", "near", "near_default", "near_nan",
    "near_infinity", "less", "less_equal", "greater", "greater_equal", "contains",
    "contains_empty", "null", "null_literal", "strings", "mixed_types", "near_mixed",
    "near_negative_tolerance", "less_nan", "less_equal_nan", "greater_nan", "greater_equal_nan",
)
EXCEPTIONS = (
    "throws_pass", "throws_derived_pass", "throws_nonstd_pass", "throws_missing_fail",
    "throws_wrong_fail", "throws_exact_pass", "throws_exact_derived_fail",
    "throws_exact_missing_fail", "throws_exact_wrong_fail", "throws_exact_nonstd_pass",
)


def expected_names(kind):
    result = {f"{name}_{outcome}" for name in CHECKS for outcome in ("pass", "fail")}
    result.difference_update(['near_nan_pass', 'near_infinity_pass', 'near_negative_tolerance_pass', 'less_nan_pass', 'less_equal_nan_pass', 'greater_nan_pass', 'greater_equal_nan_pass', 'null_literal_fail'])
    result.update(EXCEPTIONS)
    result.update(("direct_fail", "wrap_pass" if kind == "assertion" else "multiple_fail"))
    return result


class CheckTests(RunnerTest):
    def test_assertion_inventory(self):
        suite, names = self.listed_names("AssertionTests")
        self.assertEqual(suite, "AssertionTests")
        self.assertEqual(set(names), expected_names("assertion"))

    def test_expectation_inventory(self):
        suite, names = self.listed_names("ExpectationTests")
        self.assertEqual(suite, "ExpectationTests")
        self.assertEqual(set(names), expected_names("expectation"))


def helper_case(executable, kind, filename, name):
    def check(self):
        failed = name.endswith("_fail")
        _, root, case = self.single_case(executable, name, int(failed))
        self.assertEqual(root.attrib["errors"], "0")
        if failed:
            failures = self.failure(case, kind, 3 if name == "multiple_fail" else 1)
            for failure in failures:
                self.assertRegex(failure.text, rf"Location: .*{filename}:\d+")
            # Check diagnostic content, not just the existence of a failure.
            message = {
                "equal_fail": "expected 7, got 8",
                "not_equal_fail": "expected not equal to 7, got 7",
                "true_fail": "expected true, got false for 0",
                "false_fail": "expected false, got true for 42",
                "near_fail": "expected 1, got 1.25 within 0.125",
                "less_fail": "expected 2 < 2, relation was false",
                "less_equal_fail": "expected 3 <= 2, relation was false",
                "greater_fail": "expected 2 > 2, relation was false",
                "greater_equal_fail": "expected 1 >= 2, relation was false",
                "contains_fail": "container does not contain value 4",
                "contains_empty_fail": "container does not contain value 1",
                "mixed_types_fail": "expected 3, got 3.5",
                "near_mixed_fail": "expected 3, got 3.25 within 0.125",
            }.get(name)
            if message is not None:
                self.assertEqual(failures[0].attrib["message"], f"{kind.capitalize()} failed: {message}")
                self.assertIn(f"Message: {kind.capitalize()} failed: {message}", failures[0].text)
            self.assertEqual("continued" in case.findtext("system-out", ""), kind == "expectation")
        else:
            self.assertEqual(case.findall("failure"), [])
        diagnostic = {
            "throws_missing_fail": "none thrown",
            "throws_wrong_fail": "type did not match",
            "throws_exact_derived_fail": "Expected exact exception type",
            "throws_exact_missing_fail": "none thrown",
            "throws_exact_wrong_fail": "type did not match",
        }.get(name)
        if diagnostic:
            self.assertIn(diagnostic, case.find("failure").attrib["message"])
    return check


for executable, kind, filename in (
    ("AssertionTests", "assertion", "assertion_tests.cpp"),
    ("ExpectationTests", "expectation", "expectation_tests.cpp"),
):
    for name in sorted(expected_names(kind)):
        setattr(CheckTests, f"test_{kind}_{name}", helper_case(executable, kind, filename, name))
