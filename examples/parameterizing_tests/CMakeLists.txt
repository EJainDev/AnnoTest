cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
)# Enable the std module for the library
set(CMAKE_CXX_MODULE_STD 1)

project(ParameterizingTests LANGUAGES CXX)

include(CTest)

find_package(AnnoTest REQUIRED) # Discover the library

add_executable(parameterizing_tests main.cpp)
target_link_libraries(parameterizing_tests PRIVATE annotest)
target_compile_options(parameterizing_tests
                       PRIVATE -fcontract-evaluation-semantic=observe)

enable_testing()
annotest_discover_tests(parameterizing_tests) # Register tests with CTest
