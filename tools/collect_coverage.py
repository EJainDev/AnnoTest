#!/usr/bin/env python3
"""Collect gcov JSON without overwriting reports from identically named sources."""
import argparse
from pathlib import Path
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('build', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--gcov', default='gcov')
    args = parser.parse_args()
    build = args.build.resolve()
    output = args.output.resolve()
    # Audit the library and test targets, excluding dependency modules, CMake
    # compiler probes, and unrelated manual builds in the build directory.
    roots = [build / 'CMakeFiles/annotest.dir', build / 'tests/CMakeFiles']
    objects = sorted(obj for root in roots for obj in root.rglob('*.gcno'))
    if not objects:
        parser.error(f'No coverage objects found in {build}')
    if output == build or output in build.parents:
        parser.error('The output directory must not contain the build directory')
    if output.exists():
        shutil.rmtree(output)
    output.mkdir(parents=True)
    for index, obj in enumerate(objects):
        # gcov names reports after their source basename, even across targets.
        directory = output / str(index)
        directory.mkdir()
        with (directory / 'gcov.log').open('w') as log:
            subprocess.run([args.gcov, '--json-format', '-b', '-c', str(obj)],
                           cwd=directory, stdout=log, stderr=subprocess.STDOUT, check=True)
    print(f'Collected coverage from {len(objects)} objects in {output}')


if __name__ == '__main__':
    main()
