#!/usr/bin/env python3
"""Verify a relocated installation and a separate FetchContent consumer.

Run after building AnnoTest. Uses only Python's standard library. The producer's
CMake cache controls compiler, configuration and install directory overrides.
"""
import argparse
import json
import shutil
import subprocess
from pathlib import Path


def run(*args):
    print('+', ' '.join(map(str, args)), flush=True)
    subprocess.run(list(map(str, args)), check=True)


def cache_value(cache, key):
    for line in cache.splitlines():
        if line.startswith(key + ':'):
            return line.split('=', 1)[1]
    raise ValueError(f'Missing {key} in producer CMake cache')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--cmake', default='cmake')
    parser.add_argument('--work-dir', type=Path,
                        help='Isolated scratch directory (defaults to BUILD_DIR/installation-smoke)')
    parser.add_argument('--config', default='Release')
    parser.add_argument('--parallel', type=int, default=2)
    args = parser.parse_args()
    source = Path(__file__).resolve().parents[1]
    producer = args.build_dir.resolve()
    cache = (producer / 'CMakeCache.txt').read_text()
    compiler = cache_value(cache, 'CMAKE_CXX_COMPILER')
    workspace = args.work_dir.resolve() if args.work_dir else producer / 'installation-smoke'
    if (workspace == producer or workspace in producer.parents
            or workspace == source or workspace in source.parents):
        parser.error('--work-dir must not contain the source or producer build directory')
    if workspace.exists():
        shutil.rmtree(workspace)
    workspace.mkdir()
    # Spaces exercise quoting in both exported targets and test discovery.
    prefix = workspace / 'original prefix'
    relocated = workspace / 'relocated prefix'
    run(args.cmake, '--install', producer, '--config', args.config, '--prefix', prefix)
    prefix.rename(relocated)
    configs = list(relocated.rglob('AnnoTestConfig.cmake'))
    if len(configs) != 1:
        raise RuntimeError(f'Expected one installed AnnoTest config, found {configs}')
    # No exported CMake file may refer to the producer or the original prefix.
    for path in relocated.rglob('*.cmake'):
        contents = path.read_text()
        for forbidden in (str(producer), str(source), str(prefix)):
            if forbidden in contents:
                raise RuntimeError(f'Non-relocatable path {forbidden} in {path}')

    for mode in ('installed', 'fetchcontent'):
        build = workspace / mode
        options = [f'-DAnnoTest_DIR={configs[0].parent}'] if mode == 'installed' else [
            f'-DANNOTEST_SOURCE_DIR={source}']
        run(args.cmake, '-S', source / 'tests/consumer', '-B', build, '-G', 'Ninja',
            f'-DCMAKE_CXX_COMPILER={compiler}', f'-DCMAKE_BUILD_TYPE={args.config}', *options)
        run(args.cmake, '--build', build, '--config', args.config, '--parallel', args.parallel)
        ctest = str(Path(args.cmake).with_name('ctest')) if '/' in args.cmake else 'ctest'
        listing = subprocess.run([ctest, '--test-dir', str(build), '-C', args.config,
                                  '--show-only=json-v1'], check=True, capture_output=True, text=True)
        names = {test['name'] for test in json.loads(listing.stdout)['tests']}
        expected = {'ConsumerTests.arithmetic', 'ConsumerTests.contract handler',
                    'ConsumerTests.parameters_work.1', 'ConsumerTests.parameters_work.2'}
        if names != expected:
            raise RuntimeError(f'{mode} discovery mismatch: {names}')
        run(ctest, '--test-dir', build, '-C', args.config, '--output-on-failure')
    print('Relocated installation and FetchContent consumers passed', flush=True)


if __name__ == '__main__':
    main()
