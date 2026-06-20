cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
)# Enable the std module for the library
set(CMAKE_CXX_MODULE_STD 1)

project(SubtractTests LANGUAGES CXX)

include(CTest)

find_package(AnnoTest REQUIRED) # Discover the library

add_executable(subtract_tests main.cpp)
target_link_libraries(subtract_tests PRIVATE annotest)
target_compile_options(subtract_tests
                       PRIVATE -fcontract-evaluation-semantic=observe)

enable_testing()
annotest_discover_tests(subtract_tests) # Register tests with CTest
