from support import RunnerTest


class BehaviorTests(RunnerTest):
    def test_assertion_unwinds_scope(self):
        _, _, case = self.single_case("BehaviorTests", "assertion_unwinds_scope", 1)
        self.assertEqual(case.findtext("system-out"), "destroyed\n")
        self.assertEqual(self.failure(case, "assertion")[0].attrib["message"],
                         "Assertion failed: expected 17, got 29")

    def test_expectation_preserves_scope(self):
        _, _, case = self.single_case("BehaviorTests", "expectation_preserves_scope", 1)
        self.assertEqual(case.findtext("system-out"), "continued\ndestroyed\n")
        self.assertEqual(self.failure(case, "expectation")[0].attrib["message"],
                         "Expectation failed: expected 17, got 29")

    def test_fatal_after_nonfatal(self):
        _, root, case = self.single_case("BehaviorTests", "fatal_after_nonfatal", 1)
        self.assertEqual(root.attrib["failures"], "1")
        self.assertEqual([f.attrib["type"] for f in case.findall("failure")],
                         ["expectation", "assertion"])
        self.assertEqual([f.attrib["message"] for f in case.findall("failure")],
                         ["Expectation failed: expected 17, got 29",
                          "Assertion failed: expected 31, got 43"])
        self.assertNotIn("unreachable", case.findtext("system-out", ""))

    def test_passing_check_does_not_clear_failure(self):
        _, _, case = self.single_case("BehaviorTests", "passing_check_does_not_clear_failure", 1)
        self.failure(case, "expectation")
        self.assertEqual(case.findtext("system-out"), "continued\n")

    def test_failure_operands_evaluated_once(self):
        _, _, case = self.single_case("BehaviorTests", "failure_operands_evaluated_once", 1)
        self.assertEqual(case.findtext("system-out"), "counts:18:30\n")
        self.assertEqual(self.failure(case, "expectation")[0].attrib["message"],
                         "Expectation failed: expected 17, got 29")

    def test_missing_exception_callback_called_once(self):
        _, _, case = self.single_case("BehaviorTests", "missing_exception_callback_called_once", 1)
        self.failure(case, "expectation")
        self.assertEqual(case.findtext("system-out"), "count:1\n")

    def test_explicit_source_location(self):
        _, _, case = self.single_case("BehaviorTests", "explicit_source_location", 1)
        failures = case.findall("failure")
        self.assertEqual(len(failures), 2)
        for failure in failures:
            self.assertIn("Location: explicit_location.cpp:400", failure.text)

    def test_assertion_failure_does_not_stop_next_test(self):
        _, root = self.report("BehaviorTests", ["BehaviorTests.assertion_unwinds_scope",
                                              "BehaviorTests.operands_evaluated_once"], 1)
        self.assertEqual([c.attrib["name"] for c in root.findall("testcase")],
                         ["assertion_unwinds_scope", "operands_evaluated_once"])
        self.assertEqual(root.findall("testcase")[1].findall("failure"), [])


def passing_behavior(name):
    def check(self):
        _, _, case = self.single_case("BehaviorTests", name)
        self.assertEqual(case.findall("failure"), [])
    return check


for name in ("operands_evaluated_once", "exception_callbacks_called_once",
             "wrap_defers_invocation", "wrap_copies_lvalue_arguments",
             "wrap_preserves_explicit_reference", "contains_noncopyable_container",
             "string_embedded_null_is_compared"):
    setattr(BehaviorTests, f"test_{name}", passing_behavior(name))
