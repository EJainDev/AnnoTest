import json
import os
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from support import RunnerTest


class ExecutionTests(RunnerTest):
    def invoke(self, name=None, code=0, options=(), scenario='', condition='on'):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / 'result.xml'
            report = Path(directory) / 'result.json'
            env = os.environ.copy()
            env.update(ANNOTEST_SCENARIO=scenario, ANNOTEST_CONDITION=condition)
            args = [str(self.binary_dir / ('ExecutionTests.exe' if os.name == 'nt' else 'ExecutionTests')),
                    '--xml-output', str(xml), '--json-output', str(report), *options]
            if name:
                args += ['--test-name', name]
            result = subprocess.run(args, env=env, capture_output=True, text=True, timeout=20)
            self.assertEqual(result.returncode, code, result.stdout + result.stderr)
            root = ET.parse(xml).getroot()
            cases = root.findall('.//testcase')
            data = json.loads(report.read_text())
            self.assertEqual(data['tests'], len(cases))
            return result, cases, data

    def test_method_condition_precedes_constructor(self):
        _, cases, _ = self.invoke('ExecutionTests.conditional', condition='off')
        self.assertEqual(len(cases), 1)
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'method unavailable')
        self.assertNotIn('constructed', cases[0].findtext('system-out', ''))
        _, cases, _ = self.invoke('ExecutionTests.conditional')
        output = cases[0].findtext('system-out', '')
        self.assertLess(output.index('condition checked'), output.index('constructed'))
        self.assertIn('body', output)
        self.assertIn('destroyed', output)

    def test_suite_nested_and_inherited_conditions(self):
        for scenario in ('guarded', 'inherited'):
            with self.subTest(scenario=scenario):
                _, cases, _ = self.invoke(scenario=scenario, condition='off')
                self.assertTrue(cases)
                for case in cases:
                    self.assertEqual(case.find('skipped').attrib['message'], 'suite unavailable')
                    self.assertNotIn('constructed', case.findtext('system-out', ''))
        _, cases, _ = self.invoke('Guarded.Child.body', scenario='guarded')
        self.assertIn('child constructed', cases[0].findtext('system-out', ''))
        self.assertIn('cleanup', cases[0].findtext('system-out', ''))

    def test_disabled_if_and_default_reason(self):
        _, cases, _ = self.invoke(scenario='denied')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'suite denied')
        _, cases, _ = self.invoke(scenario='denied', condition='off')
        self.assertIsNone(cases[0].find('skipped'))
        _, cases, _ = self.invoke('ExecutionTests.default_reason')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'Execution condition disabled test')

    def test_conditions_short_circuit_and_disabled_tests(self):
        _, cases, _ = self.invoke('ExecutionTests.short_circuit', condition='throw')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'first condition')
        _, cases, _ = self.invoke('ExecutionTests.disabled', condition='throw')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'Disabled test')
        self.assertNotIn('condition checked', cases[0].findtext('system-out', ''))
        _, cases, _ = self.invoke('ExecutionTests.disabled', options=['--run-disabled'], condition='off')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'Execution condition not met')

    def test_condition_errors_and_skips(self):
        for mode, kind, message in (('throw', 'uncaught_exception', 'condition exception'),
                                    ('unknown', 'unknown', 'unknown exception')):
            with self.subTest(mode=mode):
                _, cases, _ = self.invoke('ExecutionTests.conditional', code=1, condition=mode)
                failure = cases[0].find('failure')
                self.assertEqual(failure.attrib['phase'], 'condition')
                self.assertEqual(failure.attrib['type'], kind)
                self.assertIn(message, failure.attrib['message'])
                self.assertNotIn('constructed', cases[0].findtext('system-out', ''))
        _, cases, _ = self.invoke('ExecutionTests.conditional', condition='skip')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'condition requested skip')

    def test_listing_does_not_evaluate_ordinary_conditions(self):
        env = os.environ.copy()
        env.update(ANNOTEST_SCENARIO='guarded', ANNOTEST_CONDITION='throw')
        binary = self.binary_dir / ('ExecutionTests.exe' if os.name == 'nt' else 'ExecutionTests')
        result = subprocess.run([str(binary), '--list'], env=env, capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('body', result.stdout)
        self.assertNotIn('condition checked', result.stdout)

    def test_conditions_apply_to_factories_before_construction(self):
        _, cases, _ = self.invoke('ExecutionTests.conditional_factory', condition='off')
        self.assertEqual(len(cases), 1)
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'factory unavailable')
        self.assertNotIn('constructed', cases[0].findtext('system-out', ''))
        _, cases, _ = self.invoke('ExecutionTests.conditional_factory')
        self.assertEqual(cases[0].attrib['name'], 'conditional_factory.one')
        self.assertIn('factory body', cases[0].findtext('system-out', ''))

    def test_repetitions_default_and_fresh_fixtures(self):
        _, cases, data = self.invoke('ExecutionTests.repeated')
        self.assertEqual(len(cases), 3)
        self.assertEqual([c.find("properties/property[@name='index']").attrib['value'] for c in cases], ['0','1','2'])
        self.assertEqual([c.find("properties/property[@name='repetition']").attrib['value'] for c in cases], ['1','2','3'])
        self.assertEqual([c['status'] for c in data['suites'][0]['tests']], ['passed']*3)
        _, cases, _ = self.invoke('ExecutionTests.group_success')
        self.assertEqual(len(cases), 1)

    def test_cli_repeat_multiplies_annotation_and_shared_fixture(self):
        _, cases, _ = self.invoke('ExecutionTests.repeated', options=['--repeat','2'])
        self.assertEqual(len(cases), 6)
        self.assertEqual([c.find("properties/property[@name='index']").attrib['value'] for c in cases], list(map(str, range(6))))
        _, cases, _ = self.invoke(scenario='shared', options=['--repeat','2'])
        self.assertEqual(len(cases), 6)

    def test_conditions_see_repetition_and_parameter_context(self):
        _, cases, _ = self.invoke('ExecutionTests.conditional_repeat')
        self.assertEqual(len(cases), 3)
        self.assertIsNotNone(cases[1].find('skipped'))
        self.assertIsNone(cases[0].find('skipped'))
        self.assertIsNone(cases[2].find('skipped'))
        _, cases, _ = self.invoke('ExecutionTests.parameterized')
        self.assertEqual(len(cases), 4)
        self.assertEqual(sum(c.find('skipped') is not None for c in cases), 2)

    def test_repeated_runtime_typed_and_dynamic_cases(self):
        for name in ('runtime_parameters', 'typed', 'dynamic'):
            with self.subTest(name=name):
                _, cases, _ = self.invoke('ExecutionTests.' + name)
                self.assertEqual(len(cases), 4)
                self.assertTrue(all(c.find('failure') is None for c in cases))

    def test_isolated_repetition_and_conditions(self):
        _, cases, _ = self.invoke('ExecutionTests.repeated', options=['--isolate'])
        self.assertEqual(len(cases), 3)
        self.assertEqual([c.find("properties/property[@name='index']").attrib['value'] for c in cases], ['0','1','2'])
        _, cases, _ = self.invoke('ExecutionTests.conditional_repeat', options=['--isolate'])
        self.assertEqual(len(cases), 3)
        self.assertIsNotNone(cases[1].find('skipped'))

    def test_repeat_failures_and_fail_fast(self):
        _, cases, _ = self.invoke('ExecutionTests.repeated_failure', code=1)
        self.assertEqual(len(cases), 3)
        _, cases, _ = self.invoke('ExecutionTests.repeated_failure', code=1, options=['--fail-fast'])
        self.assertEqual(len(cases), 1)

    def test_group_fatal_continues_blocks_then_unwinds(self):
        _, cases, _ = self.invoke('ExecutionTests.group_fatal', code=1)
        failures = cases[0].findall('failure')
        self.assertEqual([f.attrib['type'] for f in failures], ['assertion','expectation'])
        self.assertTrue(all('independent blocks' in f.attrib['message'] for f in failures))
        self.assertTrue(all('execution_tests.cpp:' in f.text for f in failures))
        output = cases[0].findtext('system-out', '')
        self.assertIn('second block', output)
        self.assertIn('third block', output)
        self.assertNotIn('unreachable', output)

    def test_group_nonfatal_continues_after_group(self):
        _, cases, _ = self.invoke('ExecutionTests.group_nonfatal', code=1)
        self.assertEqual(len(cases[0].findall('failure')), 2)
        self.assertIn('continued after group', cases[0].findtext('system-out', ''))

    def test_group_exceptions_are_independent_errors(self):
        _, cases, _ = self.invoke('ExecutionTests.group_exceptions', code=1)
        self.assertEqual(cases[0].attrib['errors'], '2')
        self.assertEqual([f.attrib['type'] for f in cases[0].findall('failure')], ['uncaught_exception','unknown'])
        self.assertIn('last block', cases[0].findtext('system-out', ''))

    def test_nested_groups_skip_and_capture(self):
        _, cases, _ = self.invoke('ExecutionTests.group_nested', code=1)
        self.assertEqual(len(cases[0].findall('failure')), 1)
        message = cases[0].find('failure').attrib['message']
        self.assertIn('Assertion group: outer', message)
        self.assertIn('Assertion group: inner', message)
        self.assertIn('outer continued', cases[0].findtext('system-out', ''))
        _, cases, _ = self.invoke('ExecutionTests.group_skip')
        self.assertEqual(cases[0].find('skipped').attrib['message'], 'group skip')
        self.invoke('ExecutionTests.group_capture')
        self.invoke('ExecutionTests.group_move_only')
        self.invoke('ExecutionTests.group_thread_capture')

    def test_repetition_limit_and_discovery_names(self):
        result = self.run_binary('ExecutionTests', ['--test-name','ExecutionTests.repeated',
                                                  '--repeat','333334'])
        self.assertEqual(result.returncode, 1)
        self.assertIn('Combined test repetitions', result.stderr)
        _, names = self.listed_names('ExecutionTests')
        self.assertEqual(names.count('repeated'), 1)
        self.assertEqual(names.count('dynamic.one'), 1)

    def test_successful_groups_outside_runner(self):
        result = self.run_binary('ExecutionTests', scenario='outside-group')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_cli_repetitions_preserve_condition_context_in_workers(self):
        for execution in ([], ['--isolate'], ['--jobs', '2']):
            with self.subTest(execution=execution):
                _, cases, data = self.invoke('ExecutionTests.conditional_repeat',
                                             options=['--repeat', '2', *execution])
                self.assertEqual(len(cases), 6)
                self.assertEqual([c.find('skipped') is not None for c in cases],
                                 [False, True, False, True, False, True])
                for index, case in enumerate(cases):
                    repetition = case.find("properties/property[@name='repetition']")
                    self.assertEqual(repetition.attrib['value'], str(index + 1))
                    output = case.findtext('system-out', '')
                    self.assertEqual('constructed' in output, index % 2 == 0)
                    if index % 2 == 0:
                        self.assertIn('destroyed', output)
                        self.assertEqual(case.find("properties/property[@name='index']").attrib['value'],
                                         str(index))
                    else:
                        self.assertIsNone(case.find("properties/property[@name='index']"))
                self.assertEqual(data['skipped'], 3)

    def test_fail_fast_ignores_skips_and_stops_after_first_failed_repetition(self):
        for execution in ([], ['--isolate']):
            with self.subTest(execution=execution):
                _, cases, _ = self.invoke(code=1, options=[
                    '--test-name', 'ExecutionTests.disabled', 'ExecutionTests.repeated_failure',
                    'ExecutionTests.group_success', '--repeat', '2', '--fail-fast', *execution])
                self.assertEqual([c.attrib['name'] for c in cases], ['disabled', 'repeated_failure'])
                self.assertIsNotNone(cases[0].find('skipped'))
                self.assertEqual(len(cases[1].findall('failure')), 1)

    def test_xml_and_json_agree_for_mixed_statuses_and_error_counts(self):
        for execution in ([], ['--isolate'], ['--jobs', '2']):
            with self.subTest(execution=execution):
                _, cases, data = self.invoke(code=1, options=[
                    '--test-name', 'ExecutionTests.group_success', 'ExecutionTests.group_nonfatal',
                    'ExecutionTests.group_exceptions', 'ExecutionTests.disabled',
                    '--repeat', '2', *execution])
                reported = [c for suite in data['suites'] for c in suite['tests']]
                self.assertEqual(len(cases), 8)
                self.assertEqual(data['tests'], 8)
                self.assertEqual(data['failed'], 4)
                self.assertEqual(data['skipped'], 2)
                self.assertEqual(len(reported), len(cases))
                self.assertEqual(sum(int(c.attrib['errors']) for c in cases), 4)
                for xml, record in zip(cases, reported):
                    self.assertEqual(record['name'], xml.attrib['name'])
                    status = ('failed' if xml.findall('failure') else
                              'skipped' if xml.find('skipped') is not None else 'passed')
                    self.assertEqual(record['status'], status)
                    self.assertEqual(record['stdout'], xml.findtext('system-out', ''))
                    self.assertEqual(record['stderr'], xml.findtext('system-err', ''))
                    skipped = xml.find('skipped')
                    self.assertEqual(record['skip_reason'],
                                     skipped.attrib['message'] if skipped is not None else '')
                    self.assertEqual(len(record['failures']), len(xml.findall('failure')))
                    for failure, node in zip(record['failures'], xml.findall('failure')):
                        self.assertEqual(failure['type'], node.attrib['type'])
                        self.assertEqual(failure['phase'], node.attrib['phase'])
                        self.assertEqual(failure['message'], node.attrib['message'])
