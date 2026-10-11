from support import RunnerTest


class ContractTests(RunnerTest):
    def test_contract_helpers_and_handler(self):
        expected = {
            "assert_pre_pass", "expect_pre_pass", "assert_post_pass", "expect_post_pass",
            "assert_inline_pass", "expect_inline_pass", "assert_no_violation_pass", "expect_no_violation_pass",
            "assert_no_violation_void_pass", "expect_no_violation_void_pass", "assert_missing_fail",
            "expect_missing_fail", "assert_unwanted_fail", "expect_unwanted_fail", "reset_pass",
            "assert_missing_value_fail", "expect_missing_value_fail", "assert_unwanted_value_fail",
            "expect_unwanted_value_fail", "custom_handler_pass", "callable_exception_fail", "unexpected_fail",
        }
        _, names = self.listed_names("ContractTests")
        self.assertEqual(set(names), expected)
        for name in sorted(expected):
            with self.subTest(name=name):
                failed = name.endswith("_fail")
                _, root, case = self.single_case("ContractTests", name, int(failed))
                if not failed:
                    self.assertEqual(case.findall("failure"), [])
                    continue
                kind = ("uncaught_exception" if name == "callable_exception_fail" else
                        "contract_violation" if name == "unexpected_fail" else
                        "expectation" if name.startswith("expect_") else "assertion")
                self.failure(case, kind)
                self.assertEqual(root.attrib["errors"], "1" if kind in ("uncaught_exception", "contract_violation") else "0")
                if "value" not in name and name not in ("callable_exception_fail",):
                    self.assertEqual("continued" in case.findtext("system-out", ""),
                                     kind in ("expectation", "contract_violation"))
