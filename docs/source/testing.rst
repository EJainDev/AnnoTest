Validation
==========

Run the self-tests
------------------

The self-test suite combines annotated C++ fixtures, independent Python report
checks, invalid-program compilation checks, and native backend checks. Deliberate
failures in the C++ fixtures are verified by the Python harness; use CTest to run
the complete suite.

.. code-block:: sh

   cmake --preset test-release-gcc-16-1 -DBUILD_EXAMPLES=ON
   cmake --build --preset build-test-release --parallel 2
   ctest --preset test-release
   python3 tools/check_installation.py --build-dir build/test-release

The tests cover annotations and discovery, assertions and expectations,
parameterization, fresh and shared fixtures, nested suites, suite hooks,
contracts, death checks, matchers, context resources, helper threads, listeners,
runtime sources and factories, registration, CLI selection, repetition,
sharding, process isolation, deadlines, and XML/JSON reporting. Checks include
passing and failing cases, cleanup after failure, skip propagation, malformed
input, and repeated or isolated execution. Invalid-program tests require the
intended diagnostic rather than accepting any compiler failure.

Linux fault-injection tests exercise output capture, child-process creation,
waiting, and worker protocol errors. Installation checks build separate consumers
from a relocated prefix and through FetchContent, including CTest discovery.
Runnable examples are also registered in CTest.

Branch coverage
---------------

Linux CI enforces at least 95% reachable runtime source-associated branch
coverage. The audit merges template instances for simple conditions and records
documented unreachable outcomes separately. Constant evaluation and invalid
declarations are verified by compilation checks. The metric covers the emitted
Linux specializations; native Windows and macOS behavior requires testing on
those platforms.

.. code-block:: sh

   cmake -S . -B build/coverage -G Ninja \
     -DBUILD_TESTS=ON -DBUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Debug \
     '-DCMAKE_CXX_FLAGS=--coverage -O0 -g' \
     -DCMAKE_EXE_LINKER_FLAGS=--coverage
   cmake --build build/coverage --parallel 2
   find build/coverage -name '*.gcda' -delete
   ctest --test-dir build/coverage --output-on-failure --no-tests=error --parallel 2
   python3 tools/collect_coverage.py build/coverage build/coverage/gcov-json
   python3 tools/coverage_audit.py build/coverage/gcov-json --minimum-percent 95 \
     --output build/coverage/reachable-source-coverage.json

Documentation
-------------

CI builds the public documentation with Sphinx warnings treated as errors.

.. code-block:: sh

   python3 -m venv .venv-docs
   .venv-docs/bin/python -m pip install -r docs/requirements.txt
   .venv-docs/bin/python -m sphinx -W --keep-going -b html \
     docs/source docs/build/html
