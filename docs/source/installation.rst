.. _installation:

Installing
==========

.. index::
   single: requirements
   single: GCC
   single: CMake
   single: Ninja

Requirements
------------

AnnoTest implements Linux, macOS, and native Windows backends. Build with GCC 16
or newer, CMake 4.3.3 or newer, and Ninja. The compiler must provide C++26
reflection, annotations, standard-library modules, and contracts. Apple Clang
and MSVC are not supported toolchains for these features.

Python 3 is needed for the self-tests and installation smoke checks. Git and
network access are needed when CMake fetches StructuralTuple and SerialXML at
the pinned commits. Installed dependency packages can be used instead.

.. index::
   single: Linux
   single: macOS
   single: Windows

Platform setup
--------------

On Linux, use GCC 16 and the repository's development container, or an equivalent
native installation of the compiler, CMake, and Ninja.

On macOS, install the Homebrew tools:

.. code-block:: sh

    brew install gcc cmake ninja python

Select ``g++-16`` explicitly; macOS's default C++ compiler is Apple Clang.

On Windows, install `MSYS2 <https://www.msys2.org/>`_ and open **UCRT64**:

.. code-block:: sh

    pacman -Syu
    # Reopen UCRT64 if requested by the updater.
    pacman -S --needed git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake \
      mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-python

Build and run from UCRT64. The binaries are native Windows ``.exe`` files.
Keep the UCRT64 ``bin`` directory on ``PATH`` when running outside this shell
so that GCC runtime DLLs can be found.

.. index::
   single: installation

Build and install
-----------------

.. code-block:: sh

    git clone https://github.com/EJainDev/AnnoTest.git
    cd AnnoTest
    cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ \
      -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
    cmake --build build --parallel 2
    ctest --test-dir build --output-on-failure
    cmake --install build --prefix /path/to/annotest

Use ``g++-16`` on macOS. Standalone builds enable examples by default; embedded
builds default to the library only. Self-tests are opt-in. Set
``BUILD_TESTS=OFF`` and ``BUILD_EXAMPLES=OFF`` for a library-only build.
``CMAKE_INSTALL_INCLUDEDIR`` and ``CMAKE_INSTALL_LIBDIR`` support custom layouts.

.. index::
   single: CMake integration
   single: annotest_discover_tests
   single: FetchContent

Installed consumer
------------------

The consumer must configure C++26 standard-library modules before ``project()``.
CMake's experimental import-std UUID depends on the CMake version:

.. code-block:: cmake

    cmake_minimum_required(VERSION 4.3.3)
    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    if(CMAKE_VERSION VERSION_LESS 4.4)
      set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
    else()
      set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
    endif()
    set(CMAKE_CXX_MODULE_STD ON)
    project(MyTests LANGUAGES CXX)
    # Enable reflection when using CMake 4.3.
    if(CMAKE_VERSION VERSION_LESS 4.4)
      string(APPEND CMAKE_CXX_FLAGS " -freflection")
    endif()
    find_package(AnnoTest CONFIG REQUIRED)
    enable_testing()
    add_executable(my_tests main.cpp)
    target_link_libraries(my_tests PRIVATE annotest::annotest)
    annotest_discover_tests(my_tests)

Pass ``-DCMAKE_PREFIX_PATH=/path/to/annotest`` when configuring the consumer.
The package installs its module sources and contract-handler header, and exports
the reflection flags and required link libraries. Prefixes can be relocated.
The same target and discovery helper are available with ``FetchContent``.

.. index::
   single: release packages

Release packages
----------------

Release archives target Linux x86-64 and contain compiled libraries, module
sources, and CMake package configuration. Use the corresponding GCC toolchain
and runtime to build consumers. Linux, macOS, and Windows are supported by
source builds.

Continue with :ref:`getting_started` for a guided first test, or use
:doc:`/api/index` to look up the public API.
