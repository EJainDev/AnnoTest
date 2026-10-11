find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(GTest CONFIG QUIET)
if(NOT GTest_FOUND)
  block(SCOPE_FOR VARIABLES)
  set(BUILD_GMOCK OFF)
  set(INSTALL_GTEST OFF)
  set(gtest_build_tests OFF)
  set(gtest_build_samples OFF)
  set(CMAKE_CXX_MODULE_STD OFF)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723)
  FetchContent_MakeAvailable(googletest)
  endblock()
  if(FETCHCONTENT_SOURCE_DIR_GOOGLETEST)
    set(ANNOTEST_BENCHMARK_GTEST_VERSION
        "source override (version unspecified)")
  else()
    set(ANNOTEST_BENCHMARK_GTEST_VERSION "1.17.0 (FetchContent)")
  endif()
else()
  set(ANNOTEST_BENCHMARK_GTEST_VERSION "${GTest_VERSION} (installed)")
endif()

set(ANNOTEST_BENCHMARK_SIZES
    "1;24;72;240"
    CACHE STRING "Total regular tests per translation unit")
set(ANNOTEST_BENCHMARK_SUITES
    3
    CACHE STRING "Fixture classes per translation unit")
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    PARENT_SCOPE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate.py")
foreach(count IN LISTS ANNOTEST_BENCHMARK_SIZES)
  if(NOT count MATCHES "^[1-9][0-9]*$")
    message(FATAL_ERROR "Benchmark sizes must be positive integers")
  endif()
  execute_process(
    COMMAND
      "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/generate.py"
      --output "${CMAKE_CURRENT_BINARY_DIR}/generated" --count "${count}"
      --suites "${ANNOTEST_BENCHMARK_SUITES}" COMMAND_ERROR_IS_FATAL ANY)
  foreach(framework annotest gtest)
    add_executable(
      ${framework}_bench_${count} EXCLUDE_FROM_ALL
      "${CMAKE_CURRENT_BINARY_DIR}/generated/${framework}_${count}.cpp")
    set_target_properties(${framework}_bench_${count} PROPERTIES CXX_EXTENSIONS
                                                                 ON)
    if(framework STREQUAL "annotest")
      target_link_libraries(${framework}_bench_${count} PRIVATE annotest)
    else()
      set_target_properties(
        ${framework}_bench_${count} PROPERTIES CXX_MODULE_STD OFF
                                               CXX_SCAN_FOR_MODULES OFF)
      target_link_libraries(${framework}_bench_${count} PRIVATE GTest::gtest)
    endif()
  endforeach()
endforeach()
file(
  WRITE "${CMAKE_CURRENT_BINARY_DIR}/metadata.json"
  "{\n  \"compiler\": \"${CMAKE_CXX_COMPILER}\",\n  \"compiler_id\": \"${CMAKE_CXX_COMPILER_ID}\",\n  \"compiler_version\": \"${CMAKE_CXX_COMPILER_VERSION}\",\n  \"build_type\": \"${CMAKE_BUILD_TYPE}\",\n  \"gtest_version\": \"${ANNOTEST_BENCHMARK_GTEST_VERSION}\",\n  \"suites\": ${ANNOTEST_BENCHMARK_SUITES}\n}\n"
)
