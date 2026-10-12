from pathlib import Path
from support import RunnerTest


class DiagnosticTests(RunnerTest):
    def test_string_success_and_pointer_equality(self):
        self.single_case('DiagnosticTests', 'string_success')

    def test_wide_string_success(self):
        self.single_case('DiagnosticTests', 'wide_string_success')

    def test_string_diagnostic_boundaries(self):
        self.single_case('DiagnosticTests', 'string_diagnostic_boundaries')

    def test_string_failures_without_messages(self):
        self.single_case('DiagnosticTests', 'string_failures_without_messages')

    def test_safe_value_printing(self):
        self.single_case('DiagnosticTests', 'safe_value_printing')

    def test_custom_printers_are_lazy_and_recursive(self):
        self.single_case('DiagnosticTests', 'custom_printers')


def string_failure(operation):
    def check(self):
        _, _, case = self.single_case('DiagnosticTests', f'string_failure.{operation + 1}', 1)
        fatal = operation >= 4
        failure = self.failure(case, 'assertion' if fatal else 'expectation')[0]
        message = failure.attrib['message']
        self.assertIn('fatal context' if fatal else 'nonfatal context', message)
        self.assertIn('strings not equal' if operation % 2 else 'strings equal', message)
        if operation % 4 >= 2:
            self.assertIn('ignoring ASCII case', message)
        if operation % 2 == 0:
            self.assertIn('First difference at index', message)
        self.assertEqual('continued' in case.findtext('system-out', ''), not fatal)
        # Ensure the helper reports its caller, not its implementation line.
        source = Path(__file__).resolve().parent.parent / 'diagnostic_tests.cpp'
        lines = source.read_text().splitlines()
        start = next(i for i, line in enumerate(lines) if f'case {operation}:' in line)
        call = next(i for i in range(start + 1, len(lines))
                    if ('expect_str' if not fatal else 'assert_str') in lines[i])
        self.assertIn(f'diagnostic_tests.cpp:{call + 1}', failure.text)
        if operation == 4:
            self.assertIn('caféΩ😀', message)
            self.assertIn('caféX😀', message)
    return check


for operation in range(8):
    setattr(DiagnosticTests, f'test_string_failure_{operation}', string_failure(operation))
