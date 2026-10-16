import os
from pathlib import Path
import tempfile
import xml.etree.ElementTree as ET
from support import RunnerTest

class ScopeTests(RunnerTest):
    def invoke(self,scenario,args=(),code=0):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'report.xml'
            result=self.run_binary('ScopeTests',['--xml-output',str(path),*args],scenario)
            self.assertEqual(result.returncode,code,result.stdout+result.stderr)
            root=ET.parse(path).getroot()
            cases=root.findall('.//testcase')
            return result,root,cases
    def test_inherited_hooks_override_and_order(self):
        _,_,cases=self.invoke('hooks')
        self.assertEqual([c.attrib['name'] for c in cases],['replaced','body'])
        expected='base setup\nderived setup a\nderived setup b\nderived cleanup b\nderived cleanup a\nbase cleanup 3\n'
        self.assertEqual([c.findtext('system-out') for c in cases],[expected,expected])
    def test_all_cleanup_hooks_run_after_failure(self):
        _,_,cases=self.invoke('cleanup',['--filter','*.body'],1)
        output=cases[0].findtext('system-out')
        self.assertIn('derived cleanup a',output)
        self.assertIn('base cleanup 3',output)
        self.assertEqual(cases[0].find('failure').attrib['phase'],'after_each')
    def test_nested_selection_constructs_outer_fixture_chain(self):
        for name in ('OuterSuite.Inner.first','OuterSuite.Inner.second','OuterSuite.Inner.Deep.body'):
            with self.subTest(name=name):
                _,_,cases=self.invoke('nested',['--test-name',name])
                self.assertEqual(len(cases),1)
                output=cases[0].findtext('system-out')
                expected=['outer constructor','outer setup','inner constructor','inner setup','inner cleanup','inner destructor','outer cleanup 10','outer destructor']
                self.assertEqual(output.splitlines(),expected)
    def test_nested_partial_setup_still_cleans_outer(self):
        for mode,phase in (('outer-setup','before_each'),('inner-constructor','constructor')):
            with self.subTest(mode=mode):
                _,_,cases=self.invoke(mode,['--test-name','OuterSuite.Inner.first'],1)
                output=cases[0].findtext('system-out')
                self.assertIn('outer cleanup 10',output)
                self.assertIn('outer destructor',output)
                self.assertNotIn('inner setup',output)
                self.assertEqual(cases[0].find('failure').attrib['phase'],phase)
    def test_shared_nested_fixture_rejects_per_test_parent_before_construction(self):
        for args in ([], ['--list'], ['--filter', '*.first'], ['--repeat', '2']):
            with self.subTest(args=args):
                result = self.run_binary('ScopeTests', args, 'nested-per-test-parent')
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn('Shared nested fixtures require a shared enclosing fixture',
                              result.stderr)
                self.assertNotIn('constructor', result.stdout)
                self.assertNotIn('destructor', result.stdout)

    def test_shared_nested_fixture_keeps_shared_parent_through_child_destruction(self):
        _, _, cases = self.invoke('nested-shared-parent')
        self.assertEqual([c.attrib['name'] for c in cases], ['first', 'second'])
        output = ''.join(c.findtext('system-out', '') for c in cases)
        self.assertEqual(output.splitlines(), ['parent constructor', 'child constructor',
                                              'child destructor 12', 'parent destructor'])

    def test_shared_context_is_stable_until_destructor(self):
        _,_,cases=self.invoke('shared')
        self.assertEqual(len(cases),2)
        paths=[c.find("properties/property[@name='fixture-directory']").attrib['value'] for c in cases]
        self.assertEqual(paths[0],paths[1])
        self.assertFalse(Path(paths[0]).exists())
        self.assertIn('shared destructor second',cases[1].findtext('system-out'))
    def test_shared_destructor_failure_is_reported(self):
        _,root,cases=self.invoke('destructor',code=1)
        self.assertEqual(root.attrib['failures'],'1')
        self.assertEqual(cases[-1].attrib['name'],'_fixture_teardown')
        self.assertEqual(cases[-1].find('failure').attrib['phase'],'destructor')
    def test_factory_has_one_fixture_and_one_pair_of_hooks(self):
        _,_,cases=self.invoke('factory')
        self.assertEqual([c.attrib['name'] for c in cases],['generated.first','generated.second'])
        output=''.join(c.findtext('system-out','') for c in cases)
        self.assertEqual(output.count('factory setup'),1)
        self.assertEqual(output.count('factory cleanup'),1)
        self.assertIn('factory cleanup 9',output)
    def test_factory_failures_are_individual_reports(self):
        for mode,phase in (('factory-setup','before_each'),('empty-factory','factory'),('duplicate-factory','factory')):
            with self.subTest(mode=mode):
                _,_,cases=self.invoke(mode,code=1)
                self.assertEqual(cases[0].find('failure').attrib['phase'],phase)
        _,_,cases=self.invoke('factory-cleanup',code=1)
        self.assertEqual(cases[-1].find('failure').attrib['phase'],'after_each')
    def test_extension_unwind_only_visits_entered_callbacks(self):
        _,_,cases=self.invoke('extension-setup',['--filter','*.passing'],1)
        self.assertEqual(cases[0].findtext('system-out').splitlines(),['extension setup 1','extension cleanup 1'])
        self.assertEqual(cases[0].find('failure').attrib['phase'],'extension_setup')
        _,_,cases=self.invoke('extension-cleanup',['--filter','*.passing'],1)
        self.assertEqual(cases[0].findtext('system-out').splitlines(),['extension setup 1','extension setup 2','extension cleanup 2','extension cleanup 1'])
    def test_observer_failures_do_not_lose_reports(self):
        for mode,phase in (('observer-end','on_test_end'),('observer-start','on_suite_start'),('observer-run-end','on_run_end'),('observer-assertion','on_assertion')):
            with self.subTest(mode=mode):
                name='failure' if mode=='observer-assertion' else 'passing'
                _,_,cases=self.invoke(mode,['--filter','*.'+name],1)
                self.assertEqual(cases[0].attrib['name'],name)
                self.assertEqual(cases[-1].find('failure').attrib['phase'],phase)
    def test_scoped_listener_never_receives_other_suites(self):
        self.invoke('scoped',['--filter','*.passing','--filter','*.first'])
    def test_resource_store_reuses_values_and_rejects_type_mismatch(self):
        self.invoke('', ['--filter','*.resources'])
    def test_empty_skip_survives_worker_protocol(self):
        _,_,cases=self.invoke('', ['--filter','*.empty_skip','--isolate'])
        self.assertIsNotNone(cases[0].find('skipped'))
        self.assertEqual(cases[0].find('skipped').attrib['message'],'')
    def test_isolated_assertions_replay_source_locations(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'events'
            previous=os.environ.get('ANNOTEST_EVENT_FILE')
            os.environ['ANNOTEST_EVENT_FILE']=str(path)
            try:
                _,_,cases=self.invoke('replay',['--filter','*.failure','--isolate'],1)
                self.assertIn('Location:',cases[0].find('failure').text)
                self.assertEqual(len(path.read_text().splitlines()),2)  # Worker and parent notifications.
            finally:
                if previous is None: os.environ.pop('ANNOTEST_EVENT_FILE',None)
                else: os.environ['ANNOTEST_EVENT_FILE']=previous

    def test_context_api_rejects_calls_outside_a_runner(self):
        result=self.run_binary('ScopeTests',[], 'outside')
        self.assertEqual(result.returncode,0,result.stdout+result.stderr)

    def test_worker_exception_categories_and_deadline_override(self):
        for name,kind in (('exception','uncaught_exception'),('unknown','unknown'),('contract','contract_violation')):
            with self.subTest(name=name):
                _,_,cases=self.invoke('', ['--filter','*.'+name,'--isolate'],1)
                self.assertEqual(cases[0].find('failure').attrib['type'],kind)
        self.invoke('', ['--filter','*.passing','--timeout','1'])
    def test_isolated_observer_failure_keeps_real_case(self):
        _,_,cases=self.invoke('observer-end',['--filter','*.passing','--isolate'],1)
        self.assertEqual(cases[0].attrib['name'],'passing')
        self.assertEqual(cases[0].find('failure').attrib['phase'],'on_test_end')
    def test_factory_listing_and_cleanup_in_worker(self):
        result=self.run_binary('ScopeTests',['--list'],'factory')
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertEqual(result.stdout.splitlines(),['DynamicSuite.','\tgenerated.first','\tgenerated.second'])
        result=self.run_binary('ScopeTests',['--list'],'factory-setup')
        self.assertEqual(result.returncode,1)
        result=self.run_binary('ScopeTests',['--list'],'factory-cleanup')
        self.assertEqual(result.returncode,1)
        _,_,cases=self.invoke('factory',['--test-name','DynamicSuite.generated.first','--isolate'])
        self.assertEqual(len(cases),1)
        self.assertIn('factory cleanup 8',cases[0].findtext('system-out'))
        _,_,cases=self.invoke('factory-cleanup',['--test-name','DynamicSuite.generated.first','--isolate'],1)
        self.assertEqual(cases[0].find('failure').attrib['phase'],'after_each')
    def test_factory_skip_preserves_reason_and_runs_cleanup(self):
        _,_,cases=self.invoke('factory-skip')
        self.assertEqual(cases[0].find('skipped').attrib['message'],'factory unavailable')
        self.assertIn('factory cleanup',cases[0].findtext('system-out'))
    def test_worker_result_write_failure_is_visible(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'missing'/'result.bin'
            result=self.run_binary('ScopeTests',['--filter','*.passing','--worker-result',str(path)],'')
            self.assertEqual(result.returncode,1)
            self.assertIn('Cannot write worker result',result.stderr)

    def test_parallel_reports_preserve_order_with_skipped_cases(self):
        _,_,cases=self.invoke('', ['--filter','*.passing','--filter','*.disabled','--jobs','2'])
        self.assertEqual([c.attrib['name'] for c in cases],['passing','disabled'])
        self.assertIsNotNone(cases[1].find('skipped'))
