from support import RunnerTest


class DeathTests(RunnerTest):
    def test_death_helpers(self):
        expected = {f"{kind}_{behavior}_{result}"
                    for kind in ("assert", "expect")
                    for behavior, result in (("exit", "pass"), ("exception", "pass"), ("signal", "pass"),
                                             ("return", "fail"), ("zero_exit", "fail"))}
        expected.update({"isolation_pass", "multiple_pass", "nested_pass", "nested_return_fail",
                         'quoted "name" café',
                         "parameter_pass.1", "parameter_pass.2"})
        _, names = self.listed_names("DeathTests")
        self.assertEqual(set(names), expected)
        for name in sorted(expected):
            with self.subTest(name=name):
                failed = name.endswith("_fail")
                _, _, case = self.single_case("DeathTests", name, int(failed))
                if failed:
                    kind = "expectation" if name.startswith("expect") else "assertion"
                    failure = self.failure(case, kind)[0]
                    self.assertIn("exited normally with status 0", failure.attrib["message"])
                    if "return" in name and name != "nested_return_fail":
                        self.assertEqual("continued" in case.findtext("system-out", ""), kind == "expectation")
                else:
                    self.assertEqual(case.findall("failure"), [])
                    if name == "nested_pass":
                        output = case.findtext("system-out", "")
                        self.assertIn("nested child\n", output)
                        self.assertIn("second nested child\n", output)
                        self.assertIn("outer survived\n", output)
