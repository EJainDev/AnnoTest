find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Fixtures intentionally include failing tests. An independent verifier checks
# their status and report instead of registering those failures directly in CTest.
foreach(name IN ITEMS Assertion Expectation Parameterization Annotation Lifecycle Contract Utility Behavior Generic)
  string(TOLOWER "${name}" source)
  add_executable(${name}Tests ${source}_tests.cpp)
  target_link_libraries(${name}Tests PRIVATE annotest)
endforeach()
target_compile_options(ContractTests PRIVATE -fcontracts -fcontract-evaluation-semantic=observe)

add_executable(InternalTests internal_tests.cpp)
target_link_libraries(InternalTests PRIVATE annotest)
set(integration_categories checks behavior generic parameterization annotations lifecycle contracts cli reporting internals)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(FaultTests fault_tests.cpp)
  target_link_libraries(FaultTests PRIVATE annotest)
  foreach(symbol IN ITEMS tmpfile dup dup2 fork waitpid)
    target_link_options(FaultTests PRIVATE "LINKER:--wrap=${symbol}")
  endforeach()
  if(CMAKE_CXX_FLAGS MATCHES "--coverage")
    target_compile_definitions(FaultTests PRIVATE ANNOTEST_COVERAGE=1)
    target_link_options(FaultTests PRIVATE "LINKER:--wrap=_Exit")
  endif()
  list(APPEND integration_categories faults)
endif()
if(UNIX OR WIN32)
  add_executable(DeathTests death_tests.cpp)
  target_link_libraries(DeathTests PRIVATE annotest)
  list(APPEND integration_categories death)
endif()
foreach(category IN LISTS integration_categories)
  set(script "${CMAKE_CURRENT_SOURCE_DIR}/integration/run_tests.py")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${script}" "${CMAKE_CURRENT_SOURCE_DIR}/integration/test_${category}.py")
  execute_process(COMMAND ${Python3_EXECUTABLE} "${script}"
    --binary-dir "${CMAKE_CURRENT_BINARY_DIR}" --category ${category} --list-tests
    OUTPUT_VARIABLE test_ids RESULT_VARIABLE discovery_result OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT discovery_result EQUAL 0)
    message(FATAL_ERROR "Cannot discover integration tests in ${category}")
  endif()
  string(REPLACE "\n" ";" test_ids "${test_ids}")
  foreach(test_id IN LISTS test_ids)
    add_test(NAME integration.${test_id}
      COMMAND ${Python3_EXECUTABLE} "${script}"
        --binary-dir $<TARGET_FILE_DIR:AssertionTests> --category ${category} --test-id ${test_id})
    set_tests_properties(integration.${test_id} PROPERTIES TIMEOUT 120 LABELS ${category})
  endforeach()
endforeach()

# Exercise the shipped discovery helper with a suite whose checks all pass.
annotest_discover_tests(UtilityTests)

foreach(invalid IN ITEMS duplicate_before_each duplicate_after_each
    test_then_before_each before_each_then_test test_then_after_each after_each_then_test)
  add_executable(Invalid_${invalid} EXCLUDE_FROM_ALL compile_fail/${invalid}.cpp)
  target_link_libraries(Invalid_${invalid} PRIVATE annotest)
  if(invalid STREQUAL "duplicate_before_each")
    set(expected "Duplicate before_each annotation")
  elseif(invalid STREQUAL "duplicate_after_each")
    set(expected "Duplicate after_each annotation")
  elseif(invalid MATCHES "^test_then_before")
    set(expected "before_each cannot be declared")
  elseif(invalid MATCHES "^test_then_after")
    set(expected "after_each cannot be declared")
  else()
    set(expected "Cannot have a `test` annotation on a lifecycle")
  endif()
  add_test(NAME compile_fail.${invalid}
    COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${CMAKE_BINARY_DIR} -DTARGET=Invalid_${invalid} "-DEXPECTED=${expected}"
      -P ${CMAKE_CURRENT_SOURCE_DIR}/expect_compile_failure.cmake)
  # Each test invokes the same build system; never run them concurrently.
  set_tests_properties(compile_fail.${invalid} PROPERTIES RUN_SERIAL TRUE TIMEOUT 100)
endforeach()

# Keep the internal assertion exception inaccessible to library consumers.
add_executable(HiddenException EXCLUDE_FROM_ALL compile_fail/assertion_error_not_exported.cpp)
target_link_libraries(HiddenException PRIVATE annotest)
add_test(NAME compile_fail.assertion_error_not_exported
  COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${CMAKE_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/expect_hidden_exception.cmake)
set_tests_properties(compile_fail.assertion_error_not_exported PROPERTIES RUN_SERIAL TRUE TIMEOUT 100)

# The native backend also builds without reflection or standard-library modules.
add_executable(PlatformTests platform_tests.cpp ../include/platform.cpp)
target_include_directories(PlatformTests PRIVATE ../include)
set_target_properties(PlatformTests PROPERTIES CXX_STANDARD 20 CXX_MODULE_STD OFF CXX_SCAN_FOR_MODULES OFF)
if(WIN32)
  target_link_libraries(PlatformTests PRIVATE shell32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(PlatformTests PRIVATE "LINKER:--wrap=backtrace_symbols")
endif()
add_test(NAME platform.native_backend COMMAND PlatformTests "--utf8=café")
set_tests_properties(platform.native_backend PROPERTIES TIMEOUT 30)
