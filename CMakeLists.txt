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
  VERSION 0.2.0
  LANGUAGES CXX)

# CMake 4.3 builds one shared std BMI and does not inherit target options.
# Append after compiler detection: GCC rejects -freflection in its default mode.
# Embedding consumers must also set this flag in their top-level directory.
if(CMAKE_VERSION VERSION_LESS 4.4 AND NOT CMAKE_CXX_FLAGS MATCHES
                                      "(^| )-freflection( |$)")
  string(APPEND CMAKE_CXX_FLAGS " -freflection")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_VERSION
                                               VERSION_LESS 16)
  message(
    FATAL_ERROR
      "AnnoTest requires GCC 16 or newer with C++26 reflection. Use Homebrew GCC on macOS or MSYS2 UCRT64 GCC on Windows."
  )
endif()

include(FetchContent)

option(BUILD_TESTS "Enable building of tests" OFF)
option(BUILD_EXAMPLES "Enable building of examples" ${PROJECT_IS_TOP_LEVEL})
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

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/AnnoTest")

# The pinned dependencies hard-code `include` in their install interfaces. Keep
# their exported include paths consistent with GNUInstallDirs overrides.
foreach(dependency IN ITEMS structural_tuple serial_xml)
  if(TARGET ${dependency})
    get_target_property(is_imported ${dependency} IMPORTED)
    if(NOT is_imported)
      get_target_property(dependency_source ${dependency} SOURCE_DIR)
      if(EXISTS "${dependency_source}/LICENSE")
        install(FILES "${dependency_source}/LICENSE"
                DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/${dependency}")
      endif()
      get_target_property(include_dirs ${dependency}
                          INTERFACE_INCLUDE_DIRECTORIES)
      string(REPLACE "$<INSTALL_INTERFACE:include>"
                     "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                     include_dirs "${include_dirs}")
      set_property(TARGET ${dependency} PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                                 "${include_dirs}")
    endif()
  endif()
endforeach()

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
         "${CMAKE_CURRENT_SOURCE_DIR}/include/sources.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/matchers.cxx"
         "${CMAKE_CURRENT_SOURCE_DIR}/include/diagnostics.cxx"
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
target_sources(
  annotest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/runner.cxx"
                   "${CMAKE_CURRENT_SOURCE_DIR}/include/platform.cpp")
target_include_directories(
  annotest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(annotest PUBLIC serial_xml::serial_xml
                                      structural_tuple::structural_tuple)
target_compile_features(annotest PUBLIC cxx_std_26)

# libstdc++ builds do not all provide std::stacktrace or the same support
# library.
if(WIN32)
  target_link_libraries(annotest PUBLIC shell32)
endif()
include(CheckCXXSourceCompiles)
block(SCOPE_FOR VARIABLES)
set(stacktrace_check
    "#include <stacktrace>\nint main() { auto s = std::stacktrace::current(); return s.size(); }"
)
check_cxx_source_compiles("${stacktrace_check}" ANNOTEST_STACKTRACE_DEFAULT)
if(NOT ANNOTEST_STACKTRACE_DEFAULT)
  set(stacktrace_available OFF)
  foreach(library IN ITEMS stdc++exp stdc++_libbacktrace)
    set(CMAKE_REQUIRED_LIBRARIES ${library})
    string(MAKE_C_IDENTIFIER "ANNOTEST_STACKTRACE_${library}" result)
    check_cxx_source_compiles("${stacktrace_check}" ${result})
    if(${result})
      set(stacktrace_available ON)
      target_link_libraries(annotest PUBLIC ${library})
      break()
    endif()
  endforeach()
  if(NOT stacktrace_available)
    target_compile_definitions(annotest PUBLIC ANNOTEST_NATIVE_STACKTRACE=1)
  endif()
endif()
endblock()

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
  CXX_MODULES_DIRECTORY cxx-modules
  DESTINATION "${ANNOTEST_INSTALL_CMAKEDIR}")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTests.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DiscoverTestsRunner.cmake
        DESTINATION "${ANNOTEST_INSTALL_CMAKEDIR}")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/annotest_contract.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/annotest_platform.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/annotest")

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
