from collections import Counter
import json
import os
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from support import RunnerTest

class ParityTests(RunnerTest):
    def invoke(self,args=(),code=0,scenario=None,env=None):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'result.xml'
            settings=os.environ.copy()
            settings.pop('ANNOTEST_SCENARIO',None)
            if scenario: settings['ANNOTEST_SCENARIO']=scenario
            if env: settings.update(env)
            result=subprocess.run([str(self.binary_dir/('ParityTests.exe' if os.name=='nt' else 'ParityTests')),
                                   '--xml-output',str(path),*args],env=settings,
                                  capture_output=True,text=True,encoding='utf-8',timeout=20)
            self.assertEqual(result.returncode,code,result.stdout+result.stderr)
            root=ET.parse(path).getroot()
            return result,root,root.findall('.//testcase') if root.tag=='testsuites' else root.findall('testcase')
    def test_fresh_instances_and_runtime_providers(self):
        _,root,cases=self.invoke(['--filter','ParityTests.fresh*','--filter','ParityTests.runtime_values*'])
        self.assertEqual(len(cases),6)
        self.assertTrue(all(not c.findall('failure') for c in cases))
        self.assertEqual({c.attrib['name'] for c in cases if c.attrib['name'].startswith('runtime_values')},
                         {'runtime_values.small','runtime_values.large'})
    def test_basic_discovery_execution(self):
        for options in ([], ['--isolate', '--jobs', '2'], ['--repeat', '2']):
            with self.subTest(options=options):
                _,_,cases=self.invoke(options,scenario='basic')
                self.assertEqual(len(cases),10 if '--repeat' in options else 5)
                self.assertEqual(sum(c.attrib['name']=='repeated' for c in cases),
                                 4 if '--repeat' in options else 2)
                for case in cases:
                    self.assertFalse(case.findall('failure'))
                    if case.attrib['name']=='disabled':
                        self.assertIsNotNone(case.find('skipped'))
                    else:
                        for name in ('cleanup','parent_cleanup'):
                            self.assertEqual(case.find(f"properties/property[@name='{name}']").attrib['value'],'yes')
        _,_,cases=self.invoke(['--filter','*.disabled','--run-disabled'],scenario='basic')
        self.assertEqual(len(cases),1)
        self.assertIsNone(cases[0].find('skipped'))
        self.assertIsNotNone(cases[0].find("properties/property[@name='cleanup']"))
    def test_basic_discovery_cleanup_after_failure(self):
        for options in ([], ['--isolate']):
            with self.subTest(options=options):
                _,_,cases=self.invoke(['--filter','*.default_argument',*options],1,
                                     scenario='basic',env={'ANNOTEST_BASIC_FAILURE':'1'})
                self.assertEqual(len(cases),1)
                self.assertIn('basic body failure',cases[0].find('failure').attrib['message'])
                for name in ('cleanup','parent_cleanup'):
                    self.assertEqual(cases[0].find(f"properties/property[@name='{name}']").attrib['value'],'yes')
    def test_nested_suites_and_dynamic_cases(self):
        _,root,cases=self.invoke(['--filter','*.child','--filter','*.generated_cases.*'])
        self.assertEqual(root.tag,'testsuites')
        self.assertEqual(len(cases),3)
        self.assertEqual({c.attrib['classname'] for c in cases},{'ParityTests','ParityTests.Nested'})
    def test_shared_fixture_option(self):
        _,_,cases=self.invoke(scenario='shared')
        self.assertEqual(len(cases),2)
    def test_constructor_and_method_injection(self):
        _,_,cases=self.invoke(['--filter','*.context'])
        path=cases[0].find("properties/property[@name='temp_dir']").attrib['value']
        self.assertFalse(Path(path).exists())
        self.invoke(scenario='injected')
    def test_runtime_skips_and_cleanup(self):
        _,root,cases=self.invoke(['--filter','*.runtime_skip','--filter','*.assumption'])
        self.assertEqual(root.attrib['skipped'],'2')
        self.assertEqual([c.find('skipped').attrib['message'] for c in cases],
                         ['GPU unavailable','network unavailable'])
        for c in cases: self.assertEqual(c.find("properties/property[@name='cleanup']").attrib['value'],'yes')
    def test_disabled_selection_and_tags(self):
        _,_,cases=self.invoke(['--tag','fast'])
        self.assertEqual([c.attrib['name'] for c in cases],['fresh_a'])
        _,_,cases=self.invoke(['--filter','*.disabled'])
        self.assertIsNotNone(cases[0].find('skipped'))
        _,_,cases=self.invoke(['--filter','*.disabled','--run-disabled'])
        self.assertIsNone(cases[0].find('skipped'))
    def test_matchers_and_inherited_methods(self):
        self.invoke(['--filter','*.matcher_examples','--filter','*.inherited'])
    def test_diagnostics_have_context_diff_and_phase(self):
        _,_,cases=self.invoke(['--filter','*.contextual_failure'],1)
        failure=cases[0].find('failure')
        self.assertIn('First difference at index 1',failure.attrib['message'])
        self.assertIn('outer context',failure.text)
        self.assertIn('input row 7',failure.text)
        self.assertEqual(failure.attrib['phase'],'test')
    def test_repeat_shuffle_and_shards(self):
        args=['--filter','*.fresh*','--repeat','2','--shuffle','--seed','42']
        a=self.invoke(args)[2];b=self.invoke(args)[2]
        self.assertEqual([c.attrib['name'] for c in a],[c.attrib['name'] for c in b])
        self.assertEqual(len(a),8)
        all_names={c.attrib['name'] for c in self.invoke(['--filter','*.fresh*'])[2]}
        partitions=[]
        for i in range(3): partitions.append({c.attrib['name'] for c in self.invoke(['--filter','*.fresh*','--shard-count','3','--shard-index',str(i)])[2]})
        self.assertEqual(set.union(*partitions),all_names)
        for i in range(3):
            for j in range(i):self.assertFalse(partitions[i]&partitions[j])
    def test_isolation_parallel_crashes_and_deadlines(self):
        self.invoke(['--filter','*.fresh*','--isolate','--jobs','2'])
        _,_,cases=self.invoke(['--filter','*.crashes','--isolate'],1,env={'ANNOTEST_CRASH':'1'})
        self.assertEqual(cases[0].find('failure').attrib['phase'],'process')
        _,_,cases=self.invoke(['--filter','*.deadline'],1,env={'ANNOTEST_HANG':'1'})
        self.assertEqual(cases[0].find('failure').attrib['phase'],'timeout')
        self.assertLess(float(cases[0].attrib['time']),3)
    def test_fail_fast_and_exclusion(self):
        _,_,cases=self.invoke(['--filter','*.cleanup_failure_after_skip','--filter','*.contextual_failure','--fail-fast'],1)
        self.assertEqual(len(cases),1)
        _,_,cases=self.invoke(['--filter','*.fresh*','--exclude','*.fresh_values.*'])
        self.assertEqual(len(cases),2)

    def test_file_sources_and_multiple_suites(self):
        self.invoke(['--filter','*.sources'])
        _,root,cases=self.invoke(scenario='multiple')
        self.assertEqual(root.tag,'testsuites')
        self.assertEqual(len(cases),3)
        self.assertEqual(root.attrib['tests'],'3')
    def test_skips_propagate_through_exception_assertions(self):
        _,_,cases=self.invoke(['--filter','*.skip_in_exception_check'])
        self.assertEqual(cases[0].find('skipped').attrib['message'],'still skipped')
        _,_,cases=self.invoke(['--filter','*.cleanup_failure_after_skip'],1)
        self.assertIsNone(cases[0].find('skipped'))
        self.assertEqual(cases[0].find('failure').attrib['phase'],'after_each')
    def test_new_options_reject_invalid_values(self):
        for args in (['--repeat','0'],['--repeat','-1'],['--jobs','0'],['--jobs','257'],
                     ['--timeout','nan'],['--timeout','0'],['--seed','bad'],
                     ['--shard-count','0'],['--shard-index','1'],['--shuffle=yes'],
                     ['--filter='],['--tag=']):
            with self.subTest(args=args):
                result=self.run_binary('ParityTests',args)
                self.assertEqual(result.returncode,1)
                self.assertIn('Error:',result.stderr)

    def test_integer_options_reject_trailing_and_overflow_input(self):
        for option in ('--repeat','--jobs','--seed','--shard-count','--shard-index'):
            for value in ('1x','9'*60):
                with self.subTest(option=option,value=value):
                    result=self.run_binary('ParityTests',[option,value])
                    self.assertEqual(result.returncode,1)
                    self.assertIn('integer',result.stderr)

    def test_json_listing_multiple_suites(self):
        result=self.run_binary('ParityTests',['--list-json'],scenario='multiple')
        self.assertEqual(result.returncode,0,result.stderr)
        data=json.loads(result.stdout)
        self.assertEqual(data['schema_version'],1)
        self.assertEqual({suite['name'] for suite in data['suites']},
                         {'SharedFixture','InjectedFixture'})
        self.assertEqual(sum(len(suite['tests']) for suite in data['suites']),3)

    def test_duplicate_suite_names_are_rejected(self):
        result=self.run_binary('ParityTests',scenario='duplicate')
        self.assertEqual(result.returncode,1)
        self.assertIn('Duplicate suite name: BasicFixture',result.stderr)

    def test_zero_seed_is_reproducible(self):
        args=['--filter','*.fresh*','--shuffle','--seed','0']
        a=self.invoke(args)[2];b=self.invoke(args)[2]
        self.assertEqual([c.attrib['name'] for c in a],[c.attrib['name'] for c in b])

    def test_matcher_negative_paths(self):
        self.invoke(['--filter','*.matcher_negative_paths'])
        result,_,cases=self.invoke(['--filter','*.matcher_failure'],1)
        failures=cases[0].findall('failure')
        self.assertEqual([f.attrib['type'] for f in failures],['expectation','assertion'])
        self.assertIn('nonfatal matcher context',failures[0].attrib['message'])
        self.assertIn('fatal matcher context',failures[1].attrib['message'])
        self.assertIn('continued after expectation',cases[0].findtext('system-out'))
        self.assertNotIn('unreachable',cases[0].findtext('system-out'))
    def test_source_edge_cases(self):
        self.invoke(['--filter','*.source_edge_cases'])
    def test_matcher_exception_dispatch(self):
        _,_,cases=self.invoke(['--test-name','ParityTests.matcher_exception_dispatch'])
        self.assertEqual(len(cases),1)
        self.assertIsNone(cases[0].find('skipped'))
        for mode in ('throws','throws-matcher','no-throw'):
            with self.subTest(mode=mode):
                _,_,cases=self.invoke(['--test-name','ParityTests.matcher_exception_dispatch'],
                                     env={'ANNOTEST_MATCHER_SKIP':mode})
                self.assertEqual(cases[0].find('skipped').attrib['message'],'propagate matcher skip')
    def test_ordered_matcher_boundaries(self):
        _,_,cases=self.invoke(['--test-name','ParityTests.ordered_matcher_boundaries'])
        self.assertEqual(len(cases),1)
    def test_unordered_matcher_boundaries(self):
        for options in ([], ['--isolate'], ['--repeat', '2']):
            with self.subTest(options=options):
                _,_,cases=self.invoke(['--test-name','ParityTests.unordered_matcher_boundaries', *options])
                self.assertEqual(len(cases), 2 if '--repeat' in options else 1)
                self.assertTrue(all(not case.findall('failure') for case in cases))
    def test_fatal_failure_inspection_boundaries(self):
        for options in ([], ['--isolate'], ['--repeat', '2']):
            with self.subTest(options=options):
                _,_,cases=self.invoke(['--test-name','ParityTests.fatal_failure_inspection_boundaries', *options])
                self.assertEqual(len(cases), 2 if '--repeat' in options else 1)
                self.assertTrue(all(not case.findall('failure') for case in cases))
    def test_source_empty_fields_and_headers(self):
        _,_,cases=self.invoke(['--test-name','ParityTests.source_empty_fields_and_headers'])
        self.assertEqual(len(cases),1)
    def test_range_difference_diagnostics(self):
        _,_,cases=self.invoke(['--test-name','ParityTests.range_difference_diagnostics'])
        self.assertEqual(len(cases),1)
    def test_matcher_diagnostics_without_custom_message(self):
        _,_,cases=self.invoke(['--filter','*.matcher_failure_empty'],1)
        failures=cases[0].findall('failure')
        self.assertEqual([f.attrib['type'] for f in failures],['expectation','assertion'])
        self.assertTrue(all('Expected 2, got 3' in f.attrib['message'] for f in failures))
    def test_matcher_skips_are_not_swallowed(self):
        for mode in ('throws','throws-matcher','no-throw'):
            with self.subTest(mode=mode):
                _,_,cases=self.invoke(['--filter','*.matcher_skip_paths'],env={'ANNOTEST_MATCHER_SKIP':mode})
                self.assertEqual(cases[0].find('skipped').attrib['message'],'matcher skip')
    def test_unseeded_shuffle_and_tag_exclusions(self):
        result,_,cases=self.invoke(['--filter','*.fresh*','--shuffle'])
        self.assertIn('Shuffle seed:',result.stdout)
        self.assertEqual(len(cases),4)
        _,_,cases=self.invoke(['--filter','*.fresh*','--exclude-tag','fast'])
        self.assertEqual(len(cases),3)
        _,_,cases=self.invoke(['--filter','*.fresh*','--exclude-tag','unit'])
        self.assertEqual(cases,[])
    def test_isolated_failure_and_skip_reports(self):
        _,_,cases=self.invoke(['--filter','*.matcher_failure','--isolate'],1)
        self.assertEqual(len(cases[0].findall('failure')),2)
        self.assertEqual([f.attrib['phase'] for f in cases[0].findall('failure')],['test','test'])
        _,_,cases=self.invoke(['--filter','*.runtime_skip','--isolate'])
        self.assertEqual(cases[0].find('skipped').attrib['message'],'GPU unavailable')

    def test_value_printers_and_exception_failure_paths(self):
        self.invoke(['--filter','*.value_printer_paths'])
        _,_,cases=self.invoke(['--filter','*.no_throw_failure'],1)
        self.assertEqual([f.attrib['type'] for f in cases[0].findall('failure')],['expectation','assertion'])
    def test_worker_protocol_errors_are_reported(self):
        for mode,message in (('missing','Invalid worker result'),('exit','Test worker crashed'),
                             ('oversized','Invalid worker result'),('truncated','Invalid worker result'),
                             ('truncated-string','Incomplete worker result'),
                             ('protocol','Invalid worker protocol'),('failure-count','Invalid worker failure count'),
                             ('failure-type','Invalid worker failure type'),('properties','Invalid worker property count'),
                             ('negative-failure-type','Invalid worker failure type'),
                             ('exit-mismatch','Worker exit status disagrees')):
            with self.subTest(mode=mode):
                _,_,cases=self.invoke(['--filter','*.fresh_a','--isolate'],1,env={'ANNOTEST_WORKER_FAULT':mode})
                failure=cases[0].find('failure')
                self.assertEqual(failure.attrib['phase'],'process')
                self.assertIn(message,failure.attrib['message'])

    def test_provider_discovery_rejects_bad_data(self):
        import subprocess
        for mode,message in (('empty','returned no cases'),('duplicate','Duplicate expanded'),
                             ('name-empty','Case names'),('name-newline','Case names')):
            with self.subTest(mode=mode):
                env=os.environ.copy();env['ANNOTEST_PROVIDER_FAULT']=mode
                result=subprocess.run([str(self.binary_dir/'ParityTests'),'--list'],env=env,capture_output=True,text=True,timeout=5)
                self.assertEqual(result.returncode,1)
                self.assertIn(message,result.stderr)
    def test_more_invalid_options(self):
        for args in (['--repeat'],['--repeat','--help'],['--timeout','-1'],['--timeout','bad'],
                     ['--timeout','inf'],['--timeout','1x'],['--repeat','1000001']):
            with self.subTest(args=args):
                result=self.run_binary('ParityTests',args)
                self.assertEqual(result.returncode,1)
                self.assertIn('Error:',result.stderr)
    def test_question_glob_and_trailing_star(self):
        _,_,cases=self.invoke(['--filter','*.fresh_?'])
        self.assertEqual(len(cases),2)
        _,_,cases=self.invoke(['--filter','ParityTests.fresh_a*'])
        self.assertEqual(len(cases),1)

    def test_shards_partition_repeated_dynamic_nested_and_disabled_cases(self):
        selection = ['--filter', '*.fresh*', '--filter', '*.runtime_values.*',
                     '--filter', '*.generated_cases.*', '--filter', '*.child',
                     '--filter', '*.disabled']
        def identities(cases):
            return Counter((c.attrib['classname'], c.attrib['name']) for c in cases)

        baseline = identities(self.invoke(selection)[2])
        self.assertEqual(sum(baseline.values()), 10)
        # More shards than cases also exercises empty shards. Membership must be
        # independent of repetition, shuffle order, and the worker backend.
        for count, execution in ((1, []), (3, []), (13, ['--jobs', '2'])):
            partitions = []
            for index in range(count):
                with self.subTest(count=count, index=index):
                    options = [*selection, '--shard-count', str(count), '--shard-index', str(index),
                               '--repeat', '2', '--shuffle', '--seed', '42', *execution]
                    cases = self.invoke(options)[2]
                    partition = identities(cases)
                    self.assertTrue(all(n == 2 for n in partition.values()))
                    self.assertTrue(set(partition) <= set(baseline))
                    for previous in partitions:
                        self.assertFalse(set(previous) & set(partition))
                    partitions.append(partition)
            self.assertEqual(sum(partitions, Counter()), Counter({key: 2 for key in baseline}))
            if count > len(baseline):
                self.assertTrue(any(not partition for partition in partitions))

    def test_selection_intersections_and_exclusion_precedence(self):
        scenarios = (
            (['--test-name', 'ParityTests.fresh_a', 'ParityTests.fresh_b',
              '--filter', '*.fresh_a'], ['fresh_a']),
            (['--filter', '*.fresh*', '--tag', 'fast'], ['fresh_a']),
            (['--filter', '*.fresh*', '--tag', 'fast', '--tag', 'missing'], ['fresh_a']),
            (['--test-name', 'ParityTests.fresh_a', '--exclude', '*.fresh_a'], []),
            (['--filter', '*.fresh*', '--tag', 'fast', '--exclude-tag', 'fast'], []),
            (['--filter', '*.fresh*', '--filter', '*.fresh_a', '--exclude', '*.fresh_values.*'],
             ['fresh_a', 'fresh_b']),
        )
        for options, expected in scenarios:
            with self.subTest(options=options):
                _, root, cases = self.invoke(options)
                self.assertEqual([c.attrib['name'] for c in cases], expected)
                self.assertEqual(int(root.attrib['tests']), len(expected))

    def test_worker_crash_and_timeout_do_not_prevent_next_case(self):
        for name, env, phase in (('crashes', {'ANNOTEST_CRASH': '1'}, 'process'),
                                 ('deadline', {'ANNOTEST_HANG': '1'}, 'timeout')):
            for execution in (['--isolate'], ['--jobs', '2']):
                with self.subTest(name=name, execution=execution):
                    _, root, cases = self.invoke(['--test-name', 'ParityTests.' + name,
                                                 'ParityTests.disabled', '--run-disabled',
                                                 *execution], 1, env=env)
                    self.assertEqual([c.attrib['name'] for c in cases], [name, 'disabled'])
                    self.assertEqual(cases[0].find('failure').attrib['phase'], phase)
                    self.assertFalse(cases[1].findall('failure'))
                    self.assertIsNone(cases[1].find('skipped'))
                    self.assertEqual(cases[1].find("properties/property[@name='ran']").attrib['value'],
                                     'yes')
                    self.assertEqual(root.attrib['failures'], '1')
