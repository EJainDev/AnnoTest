import json
import os
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from support import RunnerTest

class FeatureTests(RunnerTest):
    def invoke(self, names=(), code=0, scenario='', options=(), env=None):
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory)/'report.xml'
            settings = os.environ.copy()
            settings['ANNOTEST_SCENARIO'] = scenario
            if env: settings.update(env)
            args = [str(self.binary_dir/('FeatureTests.exe' if os.name=='nt' else 'FeatureTests')),
                    '--xml-output',str(xml),*options]
            if names: args += ['--test-name',*names]
            result = subprocess.run(args,env=settings,capture_output=True,text=True,timeout=20)
            self.assertEqual(result.returncode,code,result.stdout+result.stderr)
            root = ET.parse(xml).getroot()
            return result,root,root.findall('.//testcase')
    def test_compile_time_names(self):
        _,_,cases=self.invoke(scenario='names')
        self.assertEqual([c.attrib['name'] for c in cases],['underscores become spaces','case: custom name'])
        self.assertEqual({c.attrib['classname'] for c in cases},{'group: NamedSuite'})
    def test_parameterized_classes_and_nested_fixtures(self):
        _,_,cases=self.invoke(scenario='constructors')
        self.assertEqual(len(cases),6)
        self.assertEqual({c.attrib['classname'] for c in cases},
                         {'ConstructorCases.1','ConstructorCases.2','ConstructorCases.1.Nested','ConstructorCases.2.Nested'})
        self.invoke(['ConstructorCases.2.positive'],scenario='constructors',options=['--isolate'])
    def test_runtime_class_sources(self):
        _,_,cases=self.invoke(scenario='runtime-constructors')
        self.assertEqual({c.attrib['classname'] for c in cases},{'RuntimeConstructorCases.small','RuntimeConstructorCases.large'})
    def test_invalid_class_sources(self):
        for fault in ('empty','duplicate','invalid'):
            with self.subTest(fault=fault):
                settings=os.environ.copy();settings.update(ANNOTEST_SCENARIO='runtime-constructors',ANNOTEST_SOURCE_FAULT=fault)
                result=subprocess.run([str(self.binary_dir/'FeatureTests'),'--list'],env=settings,capture_output=True,text=True)
                self.assertEqual(result.returncode,1,result.stdout+result.stderr)
                self.assertIn('Cannot run tests',result.stderr)
    def test_typed_suites_have_distinct_names(self):
        _,_,cases=self.invoke(scenario='typed')
        self.assertEqual(len(cases),2)
        self.assertEqual(len({c.attrib['classname'] for c in cases}),2)
    def test_runtime_registration_and_factory_hooks(self):
        _,_,cases=self.invoke(scenario='registered')
        self.assertEqual([c.attrib['name'] for c in cases],['plain','factory'])
        self.assertEqual(cases[1].find("properties/property[@name='cleaned']").attrib['value'],'yes')
        self.invoke(['Registered.factory'],scenario='registered',options=['--isolate'])
    def test_failure_capture_does_not_fail_test(self):
        self.invoke(['FeatureTests.failure_interception'])
    def test_failed_interception_is_reported(self):
        _,_,cases=self.invoke(['FeatureTests.interception_failure'],1)
        self.assertEqual(len(cases[0].findall('failure')),2)
    def test_interception_checks_type_message_and_nonfatal_failures(self):
        _,_,cases=self.invoke(['FeatureTests.interception_boundaries'],1)
        failures=cases[0].findall('failure')
        self.assertEqual(len(failures),2)
        self.assertTrue(all(f.attrib['type']=='expectation' for f in failures))
        self.assertTrue(all('expected marker' in f.attrib['message'] for f in failures))
    def test_no_throw_failure_context_is_captured(self):
        _,_,cases=self.invoke(['FeatureTests.contextual_no_throw_failures'])
        self.assertEqual(len(cases),1)
    def test_no_fatal_failure_and_inspection(self):
        _,_,cases=self.invoke(['FeatureTests.no_fatal_failure'],1)
        self.assertEqual(len(cases[0].findall('failure')),4)
    def test_custom_assertion_messages(self):
        _,_,cases=self.invoke(['FeatureTests.messages'],1)
        failures=cases[0].findall('failure')
        self.assertEqual(len(failures),13)
        self.assertTrue(all('message' in f.text and 'message' in f.attrib['message'] for f in failures))
    def test_helper_threads_record_every_failure(self):
        _,_,cases=self.invoke(['FeatureTests.thread_failures'],1)
        failures=cases[0].findall('failure')
        self.assertEqual(len(failures),40)
        for i in range(4): self.assertEqual(sum(('worker '+str(i)) in f.text for f in failures),10)
    def test_listener_receives_complete_result(self):
        _,_,cases=self.invoke(['FeatureTests.details'],1)
        self.assertEqual(len(cases),1)
    def test_exit_status_signal_and_stderr(self):
        self.invoke(['FeatureTests.exit_checks'])
        self.invoke(['FeatureTests.exit_checks'],options=['--isolate'])
    def test_exit_mismatches(self):
        _,_,cases=self.invoke(['FeatureTests.exit_mismatch'],1)
        self.assertEqual(len(cases[0].findall('failure')),4)
    def test_json_report_and_brief_output(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'report.json'
            result,_,_=self.invoke(['FeatureTests.passing','FeatureTests.details','FeatureTests.skip_case'],1,
                                   options=['--brief','--json-output',str(path)])
            report=json.loads(path.read_text())
            cases=report['suites'][0]['tests']
            by_name={c['name']:c for c in cases}
            self.assertEqual([by_name[n]['status'] for n in ('passing','details','skip_case')],['passed','failed','skipped'])
            self.assertIn('result output',by_name['details']['stdout'])
            self.assertEqual(len(by_name['details']['failures']),1)
            self.assertNotIn('passing: Passed',result.stdout)
            self.assertIn('details: Failed',result.stdout)
    def test_json_write_failure(self):
        with tempfile.TemporaryDirectory() as directory:
            result,_,_=self.invoke(['FeatureTests.passing'],1,options=['--json-output',str(Path(directory)/'missing'/'report.json')])
            self.assertIn('Cannot write JSON report',result.stderr)
    def test_environment_defaults(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'report.json'
            result,_,cases=self.invoke(env={'ANNOTEST_FILTER':'*.passing','ANNOTEST_BRIEF':'1','ANNOTEST_JSON_OUTPUT':str(path)})
            self.assertEqual([c.attrib['name'] for c in cases],['passing'])
            self.assertEqual(json.loads(path.read_text())['suites'][0]['tests'][0]['status'],'passed')
            self.assertNotIn('Passed',result.stdout)
    def test_debugger_break_on_failure(self):
        if os.name=='nt': self.skipTest('Debugger breakpoint status is platform-specific')
        result=self.run_binary('FeatureTests',['--test-name','FeatureTests.details','--break-on-failure'])
        self.assertEqual(result.returncode,-5)

    def test_all_thread_failure_interception(self):
        self.invoke(['FeatureTests.all_thread_capture'])
    def test_managed_helper_threads_unwind_fatal_assertions(self):
        _,root,cases=self.invoke(['FeatureTests.managed_threads'],1)
        self.assertEqual(len(cases[0].findall('failure')),3)
        self.assertEqual(root.attrib['errors'],'2')
    def test_class_level_type_parameters(self):
        _,_,cases=self.invoke(scenario='annotated-typed')
        self.assertEqual(len(cases),2)
        self.assertEqual(len({c.attrib['classname'] for c in cases}),2)

    def test_breakpoint_can_resume(self):
        if os.name=='nt': self.skipTest('POSIX signal handler')
        self.invoke(['FeatureTests.breakpoint_handled'],1,options=['--break-on-failure'])
    def test_suite_result_observer_failure_is_reported(self):
        _,_,cases=self.invoke(['FeatureTests.passing'],1,env={'ANNOTEST_SUITE_OBSERVER_FAILURE':'1'})
        self.assertEqual(cases[-1].find('failure').attrib['phase'],'on_suite_result')
    def test_json_escapes_captured_bytes(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'result.json'
            # The fixture writes invalid UTF-8 only to captured stdout, not the console.
            self.invoke(['FeatureTests.json_bytes'],options=['--json-output',str(path)])
            report=json.loads(path.read_text())
            case=report['suites'][0]['tests'][0]
            self.assertIn('quote" slash\\ newline\n',case['stdout'])
            self.assertIn('\\xff',case['stdout'])
            self.assertEqual(case['properties'][0]['name'],'escaped"key')

    def test_environment_scalar_options_and_cli_override(self):
        env={'ANNOTEST_REPEAT':'2','ANNOTEST_SEED':'42','ANNOTEST_JOBS':'2',
             'ANNOTEST_TIMEOUT':'2','ANNOTEST_SHARD_COUNT':'1','ANNOTEST_SHARD_INDEX':'0',
             'ANNOTEST_SHUFFLE':'1','ANNOTEST_FAIL_FAST':'1','ANNOTEST_RUN_DISABLED':'1',
             'ANNOTEST_ISOLATE':'1','ANNOTEST_TAG':'missing','ANNOTEST_EXCLUDE_TAG':'ignored',
             'ANNOTEST_EXCLUDE':'*.nothing'}
        _,_,cases=self.invoke(['FeatureTests.passing'],env=env)
        self.assertEqual(cases,[])  # Included tag still applies to explicit name selection.
        env.pop('ANNOTEST_TAG')
        _,_,cases=self.invoke(['FeatureTests.passing'],env=env,options=['--repeat','1','--brief=0','--shuffle=0','--fail-fast=0','--run-disabled=0','--isolate=0'])
        self.assertEqual(len(cases),1)
    def test_environment_xml_output_can_be_overridden(self):
        with tempfile.TemporaryDirectory() as directory:
            original=Path(directory)/'original.xml'
            self.invoke(['FeatureTests.passing'],env={'ANNOTEST_XML_OUTPUT':str(original)})
            self.assertFalse(original.exists())

    def test_empty_registration_has_valid_reports(self):
        _,root,cases=self.invoke(scenario='registered-empty')
        self.assertEqual(root.tag,'testsuites')
        self.assertEqual(root.attrib['tests'],'0')
        self.assertEqual(cases,[])
        result=self.run_binary('FeatureTests',['--help'],'registered-empty')
        self.assertEqual(result.returncode,0)
    def test_duplicate_runtime_registration_is_rejected(self):
        result=self.run_binary('FeatureTests',[], 'registered-duplicates')
        self.assertEqual(result.returncode,1)
        self.assertIn('Duplicate full test name',result.stderr)
    def test_runtime_registration_merges_with_annotated_suite(self):
        _,_,cases=self.invoke(['FeatureTests.runtime'],scenario='registered-combined')
        self.assertEqual([c.attrib['name'] for c in cases],['runtime'])
    def test_runtime_fixture_factory_accepts_context(self):
        self.invoke(scenario='registered-context')
        _,_,cases=self.invoke(scenario='registered-null',code=1)
        self.assertIn('Fixture factory returned null',cases[0].find('failure').attrib['message'])
    def test_json_multi_suite_and_multiple_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'result.json'
            self.invoke(scenario='registered',options=['--json-output',str(path)])
            self.assertEqual(json.loads(path.read_text())['tests'],2)
            self.invoke(['FeatureTests.messages'],1,options=['--json-output',str(path)])
            self.assertEqual(len(json.loads(path.read_text())['suites'][0]['tests'][0]['failures']),13)
            self.invoke(scenario='names',options=['--json-output',str(path)])
            self.invoke(scenario='constructors',options=['--json-output',str(path)])
            self.assertEqual(len(json.loads(path.read_text())['suites']),4)

    def test_isolated_breakpoint_occurs_in_parent(self):
        if os.name=='nt': self.skipTest('POSIX signal handler')
        self.invoke(['FeatureTests.details'],1,options=['--isolate','--break-on-failure'],env={'ANNOTEST_HANDLE_BREAK':'1'})

    def test_runtime_registration_accepts_move_only_callables(self):
        self.invoke(scenario='registered-move-only')
