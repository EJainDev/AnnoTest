from support import RunnerTest


class FaultTests(RunnerTest):
    def test_child_return_is_failure(self):
        _, _, case = self.single_case("FaultTests", "child_return", 1)
        self.assertIn("exited normally with status 0", self.failure(case, "assertion")[0].attrib["message"])

    def test_child_exception_is_death(self):
        self.single_case("FaultTests", "child_exception")


def capture_failure(mode):
    def check(self):
        result = self.run_binary("FaultTests", ["--test-name", "FaultTests.output"], mode)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("Cannot redirect test output" if mode.startswith("redirect")
                      else "Cannot capture test output", result.stdout)
        self.assertIn("stdout restored", result.stdout)
        self.assertIn("stderr restored", result.stderr)
        self.assertNotIn("body executed", result.stdout)
    return check


def death_failure(mode, kind):
    def check(self):
        _, _, case = self.single_case("FaultTests", f"{kind}_death", 1, mode)
        failure = self.failure(case, "assertion" if kind == "assert" else "expectation")[0]
        self.assertIn("fork() failed" if mode == "fork" else "waitpid() failed",
                      failure.attrib["message"])
        self.assertEqual("continued" in case.findtext("system-out", ""), kind == "expect")
    return check


for mode in ("tmp_stdout", "tmp_stderr", "dup_stdout", "dup_stderr",
             "redirect_stdout", "redirect_stderr"):
    setattr(FaultTests, f"test_{mode}", capture_failure(mode))
for mode in ("fork", "wait"):
    for kind in ("assert", "expect"):
        setattr(FaultTests, f"test_{kind}_{mode}_failure", death_failure(mode, kind))
