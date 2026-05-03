# C++ Utils

This is a C++26 library that requires support for most C++26 features, especially reflection. There are 2 working modules, [*testing*](testing/README.md) and [*refl*](refl/README.md) which help improve test writing and provide reflection utilities respectively. Checkout their READMEs for how to use them. This project also requires support for GCC-16 or above and is customized accordingly. Both of these modules are enabled by default, but can be disabled in the CMake.

There is an in progress memory management module *memory* which can be enabled in the [CMakeLists](CMakeLists.txt)