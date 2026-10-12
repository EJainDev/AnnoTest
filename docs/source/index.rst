AnnoTest
========

.. raw:: html

   <div align="center">

.. image:: https://img.shields.io/badge/github--repo-blue?logo=github
   :target: https://github.com/EJainDev/AnnoTest
.. image:: https://shields.io/badge/License-MIT-green
   :target: https://github.com/EJainDev/AnnoTest/blob/main/LICENSE
.. image:: https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml/badge.svg
   :target: https://github.com/EJainDev/AnnoTest/actions/workflows/build_and_test.yml
.. image:: https://img.shields.io/badge/C%2B%2B-26-blue
.. image:: https://img.shields.io/github/last-commit/EJainDev/AnnoTest
   :alt: GitHub last commit

.. raw:: html

   </div>

What is AnnoTest?
-----------------

AnnoTest is a C++26 **annotation-based unit testing library**. Import the
``annotest`` module and mark public test methods with ``[[= test{}]]``.

The library supports compile-time test discovery, fixture hooks, value/type parameterization, assertions, expectations, contracts, captured output, and death tests on Linux, macOS, and native Windows.

.. code-block:: cpp

    [[= test{}]] void my_test();

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   installation
   usage/getting_started
   examples/index
   api/index
   genindex
