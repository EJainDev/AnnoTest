#!/usr/bin/env python3
"""Install a tested native build and create its platform-specific release archive."""
import argparse
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--tag', required=True)
    parser.add_argument('--platform', required=True,
                        choices=('linux-x86_64', 'macos-arm64', 'macos-x86_64', 'windows-x86_64'))
    parser.add_argument('--output-dir', type=Path, default=Path('dist'))
    parser.add_argument('--cmake', default='cmake')
    parser.add_argument('--config', default='Release')
    args = parser.parse_args()
    if not re.fullmatch(r'v[0-9][0-9A-Za-z.+_-]*', args.tag):
        parser.error('--tag must be a version tag such as v0.2.0')
    os_name = {'linux': 'linux', 'darwin': 'macos', 'win32': 'windows'}.get(sys.platform)
    machine = platform.machine().lower()
    architecture = {'amd64': 'x86_64', 'x86_64': 'x86_64', 'arm64': 'arm64',
                    'aarch64': 'arm64'}.get(machine)
    if args.platform != f'{os_name}-{architecture}':
        parser.error(f'Archive platform {args.platform} does not match native host {os_name}-{machine}')
    source = Path(__file__).resolve().parents[1]
    build = args.build_dir.resolve()
    if not (build / 'CMakeCache.txt').is_file():
        parser.error('--build-dir must contain a configured native CMake build')
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='annotest-package-', dir=output) as scratch:
        prefix = Path(scratch) / 'package'
        subprocess.run([args.cmake, '--install', str(build), '--config', args.config,
                        '--prefix', str(prefix)], check=True)
        for file in ('README.md', 'LICENSE'):
            shutil.copy2(source / file, prefix)
        name = output / f'annotest-{args.tag}-{args.platform}'
        kind = 'zip' if os_name == 'windows' else 'gztar'
        archive = shutil.make_archive(str(name), kind, scratch, 'package')
    print(f'Packaged {archive}', flush=True)


if __name__ == '__main__':
    main()
