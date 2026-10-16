.. _howto-examples-build:

Build the bundled examples
==========================

Use the source checkout to run the exact examples linked from the how-to guides.
For a new application, follow :doc:`/usage/getting_started` instead.

Configure once
--------------

Use the :doc:`/installation` toolchain. From the checkout root:

.. code-block:: sh

   cmake -S . -B build/docs-examples -G Ninja \
     -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug \
     -DBUILD_EXAMPLES=ON -DBUILD_TESTS=OFF

Git and network access are needed when dependencies are not already available.
On Windows, use UCRT64 and append ``.exe`` to executable paths.

Build one target
----------------

Each guide names its target. For example, build the value-parameter example:

.. code-block:: sh

   cmake --build build/docs-examples --target example3_parameterized

Run and inspect it
------------------

List its cases, then select one:

.. code-block:: sh

   ./build/docs-examples/examples/example3_parameterized --list
   ./build/docs-examples/examples/example3_parameterized \
     --test-name ParameterizedTests.addition.2

The selected case checks 5 + 7 against 12 and returns exit status 0.

.. seealso::

   :doc:`parameterized` explains how to add rows.
   :doc:`/api/testing/command_line` lists selection and reporting options.
