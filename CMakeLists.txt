cmake_minimum_required(VERSION 4.3.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS 4.4)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.4)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
endif()
set(CMAKE_CXX_MODULE_STD 1)

project(
  AnnoTest
  VERSION 1.0.0.0
  LANGUAGES CXX)

include(FetchContent)

option(BUILD_TESTS "Enable building of tests" OFF)
option(BUILD_EXAMPLES "Enable building of examples" ON)
option(BUILD_BENCHMARKS "Build compile-time comparisons against GoogleTest" OFF)

# These dependencies use the same option names. Scope their optional targets so
# fetching libraries does not alter AnnoTest's build choices.
block(SCOPE_FOR VARIABLES)
set(BUILD_TESTS OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_BENCHMARKS OFF)

find_package(StructuralTuple QUIET)
if(NOT StructuralTuple_FOUND)
  FetchContent_Declare(
    structural_tuple
    GIT_REPOSITORY https://github.com/EJainDev/StructuralTuple.git
    GIT_TAG 7ad88be28ceece1d6d41da72754c6cf2c969fd6d)
  FetchContent_MakeAvailable(structural_tuple)
  target_compile_options(structural_tuple PUBLIC -freflection)
  if(NOT TARGET structural_tuple::structural_tuple)
    add_library(structural_tuple::structural_tuple ALIAS structural_tuple)
  endif()
endif()

find_package(SerialXML QUIET)
if(NOT SerialXML_FOUND)
  FetchContent_Declare(
    serial_xml
    GIT_REPOSITORY https://github.com/EJainDev/SerialXML.git
    GIT_TAG 9de4d3a13756df6033a3820470761f4ba5c41380)
  FetchContent_MakeAvailable(serial_xml)
endif()

endblock()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
endif()

# Library

add_library(annotest STATIC)
add_library(annotest::annotest ALIAS annotest)
target_compile_options(annotest PUBLIC -freflection)
target_sources(
  annotest
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}/include"
         FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/include/structs.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/exceptions.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/asserts.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/expects.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/death_test.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/test.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/posix.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/utils.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/contracts.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/logger.cxx")
target_sources(annotest
               PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/runner.cxx")
target_include_directories(
  annotest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                  $<INSTALL_INTERFACE:include>)
target_link_libraries(
  annotest PUBLIC serial_xml::serial_xml structural_tuple::structural_tuple
                  stdc++exp)
target_compile_features(annotest PUBLIC cxx_std_26)

# Installation configuration

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake)

install(
  TARGETS annotest
  EXPORT annotest_targets
  FILE_SET modules
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/annotest
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(ANNOTEST_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/AnnoTest")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AnnoTestConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfig.cmake"
  INSTALL_DESTINATION "${ANNOTEST_INSTALL_CMAKEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT annotest_targets
  NAMESPACE annotest::
  DESTINATION "${ANNOTEST_INSTALL_CMAKEDIR}")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTests.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/AnnoTest")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/annotest_contract.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/AnnoTestConfigVersion.cmake"
        DESTINATION "${ANNOTEST_INSTALL_CMAKEDIR}")

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
