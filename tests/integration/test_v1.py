import json
from pathlib import Path
import tempfile
from support import RunnerTest


class V1Tests(RunnerTest):
    def invoke(self, scenario='basic', options=(), code=0):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / 'report.xml'
            report = Path(directory) / 'report.json'
            result = self.run_binary('V1Tests', ['--xml-output', str(xml),
                                     '--json-output', str(report), *options], scenario)
            self.assertEqual(result.returncode, code, result.stdout + result.stderr)
            data = json.loads(report.read_text())
            self.assertEqual(data['schema_version'], 1)
            return result, data, [case for suite in data['suites'] for case in suite['tests']]

    @staticmethod
    def output(cases):
        return ''.join(case['stdout'] for case in cases)

    def test_suite_hooks_fresh_fixtures_and_repetitions(self):
        _, data, cases = self.invoke(options=['--filter', 'v1.data.*', '--repeat', '2'])
        self.assertEqual(data['tests'], 8)
        output = self.output(cases)
        for text in ('base before_all', 'derived before_all', 'derived after_all', 'base after_all'):
            self.assertEqual(output.count(text), 1, output)
        self.assertEqual(output.count('constructed'), 8)
        self.assertEqual(output.count('destroyed'), 8)
        self.assertLess(output.index('base before_all'), output.index('derived before_all'))
        self.assertLess(output.index('derived after_all'), output.index('base after_all'))
        self.assertTrue(all(case['tags'] == ['v1', 'data'] for case in cases))
        self.assertTrue(all(case['parameters'] for case in cases))
        self.assertTrue(all(any(p['name'] == 'suite_resource' and p['value'] == 'shared'
                               for p in case['properties']) for case in cases))

    def test_suite_hooks_shared_fixture_and_shuffle(self):
        _, _, cases = self.invoke('shared', ['--repeat', '2', '--shuffle', '--seed', '42'])
        counters = [int(p['value']) for c in cases for p in c['properties'] if p['name'] == 'counter']
        self.assertEqual(counters, list(range(1, 7)))
        self.assertEqual(self.output(cases).count('base before_all'), 1)
        self.assertEqual(self.output(cases).count('base after_all'), 1)

    def test_nested_selection_and_cleanup_order(self):
        _, _, cases = self.invoke('nested', ['--filter', 'outer.inner.*', '--repeat', '2'])
        output = self.output(cases)
        self.assertEqual(len(cases), 4)
        self.assertNotIn('outer body', output)
        self.assertEqual(output.count('outer before_all'), 1)
        self.assertEqual(output.count('inner before_all'), 1)
        self.assertLess(output.index('outer before_all'), output.index('inner before_all'))
        self.assertLess(output.index('inner after_all'), output.index('outer after_all'))
        self.assertEqual(output.count('outer before_each'), 4)
        self.assertEqual(output.count('outer after_each'), 4)

    def test_isolated_parallel_nested_and_repetitions(self):
        _, _, cases = self.invoke('nested', ['--filter', 'outer.inner.*', '--jobs', '2',
                                           '--repeat', '2', '--timeout', '5'])
        self.assertEqual(len(cases), 4)
        for case in cases:
            for text in ('outer before_all', 'inner before_all', 'inner after_all', 'outer after_all'):
                self.assertEqual(case['stdout'].count(text), 1, case)

    def test_setup_failures_prevent_bodies_and_cleanup_runs(self):
        for scenario in ('fatal', 'nonfatal', 'throw'):
            with self.subTest(scenario=scenario):
                _, data, cases = self.invoke(scenario, code=1)
                self.assertEqual(data['failed'], 2)
                self.assertNotIn('body executed', self.output(cases))
                self.assertEqual(self.output(cases).count('suite setup attempted'), 1)
                self.assertEqual(self.output(cases).count('suite cleanup attempted'), 1)
                self.assertTrue(all(c['failures'][0]['phase'] == 'before_all' for c in cases))
                if scenario != 'throw':
                    self.assertTrue(all(c['failures'][0]['location']['line'] > 0 for c in cases))

    def test_skip_still_cleans_up(self):
        _, data, cases = self.invoke('skip')
        self.assertEqual(data['skipped'], 2)
        self.assertEqual(self.output(cases).count('suite cleanup attempted'), 1)
        self.assertTrue(all(c['skip_reason'] == 'suite unavailable' for c in cases))

    def test_cleanup_exception_and_remaining_hooks(self):
        _, data, cases = self.invoke('cleanup', code=1)
        self.assertEqual(data['failed'], 1)
        self.assertEqual(cases[-1]['failures'][0]['phase'], 'after_all')
        self.assertEqual(self.output(cases).count('later cleanup attempted'), 1)

    def test_fail_fast_setup_and_worker_cleanup(self):
        for options in (['--fail-fast'], ['--isolate', '--fail-fast']):
            with self.subTest(options=options):
                _, data, cases = self.invoke('fatal', options, 1)
                self.assertEqual(data['tests'], 1)
                self.assertEqual(self.output(cases).count('suite cleanup attempted'), 1)

    def test_listing_does_not_run_ordinary_hooks(self):
        result = self.run_binary('V1Tests', ['--list-json'])
        self.assertEqual(result.returncode, 0, result.stderr)
        data = json.loads(result.stdout)
        self.assertEqual(data['schema_version'], 1)
        cases = data['suites'][0]['tests']
        self.assertEqual(len(cases), 7)
        self.assertTrue(all(case['line'] > 0 for case in cases))
        self.assertEqual(cases[0]['full_name'], 'v1.data.small')
        self.assertEqual(cases[0]['tags'], ['v1', 'data'])
        self.assertTrue(cases[0]['parameters'])
        self.assertTrue(cases[2]['parameters'])
        self.assertTrue(cases[4]['parameters'].startswith('types:'))
        self.assertTrue(cases[-1]['disabled'])
        invalid = self.run_binary('V1Tests', ['--list-json=1'])
        self.assertEqual(invalid.returncode, 1)

    def test_dynamic_discovery_and_filtered_repetitions(self):
        listing = self.run_binary('V1Tests', ['--list-json'], 'factory')
        self.assertEqual(listing.returncode, 0, listing.stderr)
        self.assertEqual([c['name'] for c in json.loads(listing.stdout)['suites'][0]['tests']],
                         ['cases.one', 'cases.two'])
        _, data, cases = self.invoke('factory', ['--filter', '*.two', '--repeat', '3'])
        self.assertEqual(data['tests'], 3)
        self.assertTrue(all(c['name'] == 'cases.two' for c in cases))
        self.assertEqual(self.output(cases).count('base before_all'), 1)
        self.assertEqual(self.output(cases).count('base after_all'), 1)

    def test_listener_events_from_threads_and_workers(self):
        for options in ([], ['--jobs', '2', '--timeout', '5']):
            with self.subTest(options=options):
                result, data, cases = self.invoke('threads', options, 1)
                self.assertEqual(data['tests'], 2)
                self.assertIn('listener assertions 4', result.stdout)
                self.assertTrue(all(len(c['failures']) == 2 for c in cases))
                self.assertTrue(all(f['location']['line'] > 0 for c in cases for f in c['failures']))

    def test_registered_fixture_suite_hooks(self):
        for options in (['--repeat', '2'], ['--jobs', '2']):
            with self.subTest(options=options):
                _, data, cases = self.invoke('registered', options)
                self.assertEqual(data['tests'], 4 if '--repeat' in options else 2)
                expected = 1 if '--repeat' in options else 2
                self.assertEqual(self.output(cases).count('base before_all'), expected)
                self.assertEqual(self.output(cases).count('base after_all'), expected)

    def test_empty_selection_and_disabled_case_preserve_success(self):
        for options in (['--filter', 'missing.*'], ['--test-name', 'v1.disabled']):
            with self.subTest(options=options):
                _, _, cases = self.invoke(options=options)
                self.assertNotIn('before_all', self.output(cases))
                self.assertNotIn('after_all', self.output(cases))

    def test_shared_nested_repetition_in_process_and_workers(self):
        for options in (['--repeat', '3', '--shuffle', '--seed', '42'],
                        ['--repeat', '3', '--jobs', '2', '--timeout', '5']):
            with self.subTest(options=options):
                _, data, cases = self.invoke('shared-nested', options)
                self.assertEqual(data['tests'], 6)
                expected = 6 if '--jobs' in options else 1
                self.assertEqual(self.output(cases).count('shared outer setup'), expected)
                self.assertEqual(self.output(cases).count('shared outer cleanup'), expected)

    def test_factories_in_workers_and_repetitions(self):
        _, data, cases = self.invoke('factory', ['--filter', '*.two', '--repeat', '2', '--jobs', '2'])
        self.assertEqual(data['tests'], 2)
        self.assertTrue(all(c['name'] == 'cases.two' for c in cases))

    def test_enclosing_suite_cleanup_failure_reaches_worker_result(self):
        for options in (['--isolate'], ['--jobs', '2']):
            with self.subTest(options=options):
                _, data, cases = self.invoke('nested-cleanup',
                    ['--filter', 'outer.inner.*', *options], 1)
                self.assertEqual(data['tests'], 2)
                self.assertEqual(data['failed'], 2)
                self.assertTrue(all(c['failures'][-1]['phase'] == 'after_all' for c in cases))
                self.assertTrue(all('outer cleanup failure' in c['failures'][-1]['message'] for c in cases))

    def test_suite_hooks_preserve_extension_traces(self):
        for options in ([], ['--jobs', '2']):
            with self.subTest(options=options):
                _, data, cases = self.invoke('traces', options, 1)
                self.assertEqual(data['tests'], 2)
                for case in cases:
                    self.assertIn('extension trace', case['failures'][0]['message'])
                    self.assertNotIn('suite-only trace', case['failures'][0]['message'])
