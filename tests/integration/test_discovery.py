"""Exercise discovery failures and real CTest invocation without reflection."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from support import RunnerTest

SOURCE = Path(__file__).resolve().parents[2]


class DiscoveryTests(RunnerTest):
    def command(self, *arguments, env=None):
        return subprocess.run(list(map(str, arguments)), capture_output=True, text=True,
                              encoding='utf-8', env=env, timeout=30)

    def discover(self, directory, scenario='valid', executable=None, timeout=30):
        cmake = os.environ.get('ANNOTEST_CMAKE', 'cmake')
        fixture = self.binary_dir / ('DiscoveryFixture.exe' if os.name == 'nt' else 'DiscoveryFixture')
        manifest = Path(directory) / 'manifest.cmake'
        env = os.environ.copy()
        env['ANNOTEST_DISCOVERY_SCENARIO'] = scenario
        result = self.command(cmake, f'-DTEST_EXECUTABLE={executable or fixture}',
                              f'-DOUTPUT_FILE={manifest}', f'-DDISCOVERY_TIMEOUT={timeout}',
                              '-P', SOURCE / 'cmake/DiscoverTests.cmake', env=env)
        return result, manifest

    def verify_manifest(self, directory, manifest, names):
        directory = Path(directory)
        (directory / 'CTestTestfile.cmake').write_text(
            f'include([==[{manifest.as_posix()}]==])\n', encoding='utf-8')
        ctest = str(Path(os.environ.get('ANNOTEST_CMAKE', 'cmake')).with_name(
            'ctest.exe' if os.name == 'nt' else 'ctest'))
        result = self.command(ctest, '--test-dir', directory, '--show-only=json-v1')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        tests = json.loads(result.stdout)['tests']
        self.assertEqual([test['name'] for test in tests], names)
        for test in tests:
            self.assertEqual(test['command'][-1], '--test-name=' + test['name'])
        result = self.command(ctest, '--test-dir', directory, '--output-on-failure', '--no-tests=error')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_discovery_failures_remove_stale_manifests(self):
        failures = {'failed': 'deliberate listing failure', 'empty': 'found no tests',
                    'suite_only': 'found no tests', 'malformed': 'Malformed',
                    'orphan': 'Malformed', 'duplicate': 'Duplicate'}
        for scenario, diagnostic in failures.items():
            with self.subTest(scenario=scenario), tempfile.TemporaryDirectory() as directory:
                manifest = Path(directory) / 'manifest.cmake'
                manifest.write_text('old manifest')
                result, manifest = self.discover(directory, scenario)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(diagnostic, result.stderr)
                self.assertFalse(manifest.exists())

    def test_missing_executable(self):
        with tempfile.TemporaryDirectory() as directory:
            result, manifest = self.discover(directory, executable=Path(directory) / 'missing')
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('executable is missing', result.stderr)
            self.assertFalse(manifest.exists())

    def test_listing_timeout(self):
        with tempfile.TemporaryDirectory() as directory:
            result, manifest = self.discover(directory, 'timeout', timeout=1)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('discovery failed', result.stderr)
            self.assertIn('timeout', result.stderr.lower())
            self.assertFalse(manifest.exists())

    def test_crlf_listing(self):
        with tempfile.TemporaryDirectory() as directory:
            result, manifest = self.discover(directory, 'crlf')
            self.assertEqual(result.returncode, 0, result.stderr)
            self.verify_manifest(directory, manifest, ['Fixture.ok'])

    def test_leading_hyphen_name(self):
        with tempfile.TemporaryDirectory() as directory:
            result, manifest = self.discover(directory, 'leading')
            self.assertEqual(result.returncode, 0, result.stderr)
            self.verify_manifest(directory, manifest, ['-Fixture.ok'])

    def test_special_names_and_executable_path(self):
        with tempfile.TemporaryDirectory(prefix='annotest discovery ') as directory:
            fixture = self.binary_dir / ('DiscoveryFixture.exe' if os.name == 'nt' else 'DiscoveryFixture')
            copied = Path(directory) / fixture.name
            shutil.copy2(fixture, copied)
            result, manifest = self.discover(directory, 'special', copied)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.verify_manifest(directory, manifest, [
                'Fixture.quote "name"; $ [=[ ]=] \\ ending.', 'Fixture.  padded  '])

    def test_ctest_requires_build_and_discovery(self):
        self.check_guard('Ninja')

    def test_multi_configuration_guards(self):
        self.check_guard('Ninja Multi-Config')

    def check_guard(self, generator):
        cmake = os.environ.get('ANNOTEST_CMAKE', 'cmake')
        ctest = str(Path(cmake).with_name('ctest.exe' if os.name == 'nt' else 'ctest'))
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / 'source'
            source.mkdir()
            build = Path(directory) / 'build'
            (source / 'CMakeLists.txt').write_text(f'''cmake_minimum_required(VERSION 4.3.3)
project(Guard LANGUAGES CXX)
enable_testing()
include([==[{(SOURCE / 'cmake/DiscoverTestsRunner.cmake').as_posix()}]==])
add_executable(guard_fixture [==[{(SOURCE / 'tests/discovery_fixture.cpp').as_posix()}]==])
set_target_properties(guard_fixture PROPERTIES CXX_STANDARD 20 CXX_SCAN_FOR_MODULES OFF)
annotest_discover_tests(guard_fixture)
''', encoding='utf-8')
            result = self.command(cmake, '-S', source, '-B', build, '-G', generator,
                                  '-DCMAKE_CXX_COMPILER=' + os.environ['ANNOTEST_COMPILER'])
            self.assertEqual(result.returncode, 0, result.stderr)
            result = self.command(ctest, '--test-dir', build, '-C', 'Debug', '--show-only=json-v1')
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('Build target guard_fixture', result.stderr)
            result = self.command(cmake, '--build', build, '--config', 'Debug')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = self.command(ctest, '--test-dir', build, '-C', 'Debug', '--output-on-failure')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            if generator == 'Ninja Multi-Config':
                result = self.command(ctest, '--test-dir', build, '-C', 'Release', '--show-only=json-v1')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('executable is missing', result.stderr)
                result = self.command(cmake, '--build', build, '--config', 'Release')
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                result = self.command(ctest, '--test-dir', build, '-C', 'Release', '--output-on-failure')
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            configuration = 'Debug' if generator == 'Ninja Multi-Config' else ''
            (build / f'guard_fixture_{configuration}_discovered_tests.cmake').unlink()
            result = self.command(ctest, '--test-dir', build, '-C', 'Debug', '--show-only=json-v1')
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('manifest is missing', result.stderr)

    def test_invalid_target_and_options_fail_configuration(self):
        cmake = os.environ.get('ANNOTEST_CMAKE', 'cmake')
        fixture_source = (SOURCE / 'tests/discovery_fixture.cpp').as_posix()
        local = f'add_executable(fixture [==[{fixture_source}]==])\n'
        cases = [
            ('', 'unknown', 'target does not exist'),
            ('add_library(fixture INTERFACE)\n', 'fixture', 'locally built executable'),
            ('add_executable(fixture IMPORTED)\n', 'fixture', 'locally built executable'),
            ('add_executable(real IMPORTED)\nadd_executable(fixture ALIAS real)\n',
             'fixture', 'locally built executable'),
            (local, 'fixture DISCOVERY_TIMEOUT 0', 'positive integer'),
            (local, 'fixture DISCOVERY_TIMEOUT', 'Invalid annotest_discover_tests arguments'),
            (local, 'fixture UNKNOWN_OPTION', 'Invalid annotest_discover_tests arguments'),
        ]
        for declaration, arguments, diagnostic in cases:
            with self.subTest(arguments=arguments), tempfile.TemporaryDirectory() as directory:
                source = Path(directory)
                (source / 'CMakeLists.txt').write_text(
                    'cmake_minimum_required(VERSION 4.3.3)\nproject(Invalid NONE)\n'
                    + f'include([==[{(SOURCE / "cmake/DiscoverTestsRunner.cmake").as_posix()}]==])\n'
                    + declaration + f'annotest_discover_tests({arguments})\n', encoding='utf-8')
                result = self.command(cmake, '-S', source, '-B', source / 'build')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(diagnostic, result.stderr)
