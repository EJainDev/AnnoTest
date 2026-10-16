from support import RunnerTest


class InternalTests(RunnerTest):
    pass


def invariant(name):
    def check(self):
        result = self.run_binary("InternalTests", [name])
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn(f"verified: {name}", result.stdout)
        if name == "help_without_program":
            self.assertIn("Usage: tests", result.stdout)
        if name == "unrunnable_plan":
            self.assertIn("has no runnable callback", result.stderr)
        if name.startswith("null_"):
            self.assertIn("Error:", result.stderr)
    return check


for name in ("logger_without_test", "unicode_string_boundaries", "grouped_assertion_outcomes", "optional_failure_fields", "failure_type_formatting",
             "unrecorded_assertion", "null_argv", "null_argument", "null_xml_path",
             "null_test_name", "help_without_program", "empty_arguments", "unrunnable_plan", "xml_snapshot_preserves_raw_log"):
    setattr(InternalTests, f"test_{name}", invariant(name))
