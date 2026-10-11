"""List or run independent CTest scenarios using only Python's standard library."""
import sys

sys.dont_write_bytecode = True

import argparse
from pathlib import Path
import unittest

from support import RunnerTest


def individual_tests(suite):
    for test in suite:
        if isinstance(test, unittest.TestSuite):
            yield from individual_tests(test)
        else:
            yield test


parser = argparse.ArgumentParser()
parser.add_argument("--binary-dir", type=Path, required=True)
parser.add_argument("--category", required=True)
parser.add_argument("--list-tests", action="store_true")
parser.add_argument("--test-id")
args = parser.parse_args()
RunnerTest.binary_dir = args.binary_dir.resolve()
suite = unittest.defaultTestLoader.discover(str(Path(__file__).parent), pattern=f"test_{args.category}.py")
if not suite.countTestCases():
    parser.error(f"No tests in category {args.category}")
if args.list_tests:
    for test in individual_tests(suite):
        print(test.id())
    raise SystemExit(0)
if args.test_id:
    selected = [test for test in individual_tests(suite) if test.id() == args.test_id]
    if len(selected) != 1:
        parser.error(f"Expected exactly one test for {args.test_id}")
    suite = unittest.TestSuite(selected)
result = unittest.TextTestRunner(verbosity=2).run(suite)
raise SystemExit(not result.wasSuccessful())
