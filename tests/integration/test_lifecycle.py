from support import RunnerTest


class LifecycleTests(RunnerTest):
    def test_fresh_instance_and_hook_order(self):
        _, root = self.report("LifecycleTests", ["LifecycleTests.passing", "LifecycleTests.second"])
        self.assertEqual([c.findtext("system-out") for c in root.findall("testcase")],
                         ["setup 1\nbody 1\ncleanup 1\n", "setup 1\nsecond 1\ncleanup 1\n"])

    def test_setup_failures_skip_body_and_run_cleanup(self):
        for mode, kind, body in (("setup_exception", "uncaught_exception", False),
                                 ("setup_assertion", "assertion", False),
                                 ("setup_expectation", "expectation", True),
                                 ("setup_unknown", "unknown", False)):
            with self.subTest(mode=mode):
                _, root, case = self.single_case("LifecycleTests", "passing", 1, mode)
                failure = self.failure(case, kind)[0]
                output = case.findtext("system-out")
                self.assertEqual("body 1" in output, body)
                self.assertIn("cleanup 1", output)
                self.assertEqual(root.attrib["errors"], "1" if kind in ("uncaught_exception", "unknown") else "0")
                if kind in ("uncaught_exception", "unknown"):
                    self.assertIn("before_each:", failure.attrib["message"])

    def test_cleanup_failures_are_reported(self):
        for mode, kind in (("cleanup_exception", "uncaught_exception"),
                           ("cleanup_assertion", "assertion"),
                           ("cleanup_expectation", "expectation"), ("cleanup_unknown", "unknown")):
            with self.subTest(mode=mode):
                _, _, case = self.single_case("LifecycleTests", "passing", 1, mode)
                failure = self.failure(case, kind)[0]
                self.assertIn("body 1", case.findtext("system-out"))
                self.assertIn("cleanup 1", case.findtext("system-out"))
                if kind in ("uncaught_exception", "unknown"):
                    self.assertIn("after_each:", failure.attrib["message"])

    def test_test_body_failures_and_cleanup(self):
        for name, kind in (("assertion_fail", "assertion"), ("expectation_fail", "expectation"),
                           ("exception_fail", "uncaught_exception"), ("unknown_fail", "unknown"),
                           ("unexpected_contract", "contract_violation")):
            with self.subTest(name=name):
                _, _, case = self.single_case("LifecycleTests", name, 1)
                self.failure(case, kind)
                output = case.findtext("system-out")
                self.assertIn("cleanup 1", output)
                self.assertNotIn("unreachable", output)
                if name == "expectation_fail":
                    self.assertIn("continued", output)

    def test_cleanup_still_runs_after_assertion_and_records_both_failures(self):
        _, root, case = self.single_case("LifecycleTests", "assertion_fail", 1, "cleanup_exception")
        self.assertEqual([f.attrib["type"] for f in case.findall("failure")],
                         ["assertion", "uncaught_exception"])
        self.assertEqual(root.attrib["failures"], "1")
        self.assertEqual(root.attrib["errors"], "1")

    def test_failure_does_not_stop_suite(self):
        _, root = self.report("LifecycleTests", ["LifecycleTests.assertion_fail", "LifecycleTests.last"], 1)
        cases = root.findall("testcase")
        self.assertEqual(len(cases), 2)
        self.assertIn("last 1", cases[1].findtext("system-out"))
        self.assertEqual(cases[1].findall("failure"), [])

    def test_disabled_test_has_no_hooks(self):
        _, root = self.report("LifecycleTests", ["LifecycleTests.disabled", "LifecycleTests.second"])
        self.assertEqual(root.attrib["skipped"], "1")
        self.assertEqual(root.find("testcase[@name='second']").findtext("system-out"), "setup 1\nsecond 1\ncleanup 1\n")

    def test_contract_flag_is_reset_between_tests(self):
        _, root = self.report("LifecycleTests", ["LifecycleTests.unexpected_contract", "LifecycleTests.last"], 1)
        self.assertEqual(root.attrib["errors"], "1")
        self.assertEqual(root.findall("testcase")[1].findall("failure"), [])

