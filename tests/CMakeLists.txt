find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Fixtures intentionally include failing tests. An independent verifier checks
# their status and report instead of registering those failures directly in CTest.
foreach(name IN ITEMS Assertion Expectation Parameterization Annotation Lifecycle Contract Utility Behavior Generic Name Parity Scope Feature Diagnostic Execution V1)
  string(TOLOWER "${name}" source)
  add_executable(${name}Tests ${source}_tests.cpp)
  target_link_libraries(${name}Tests PRIVATE annotest)
endforeach()
target_compile_options(ContractTests PRIVATE -fcontracts -fcontract-evaluation-semantic=observe)

add_executable(InternalTests internal_tests.cpp)
target_link_libraries(InternalTests PRIVATE annotest)
set(integration_categories checks behavior generic parameterization annotations lifecycle contracts cli reporting internals discovery parity scopes features diagnostics execution v1)
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
  add_executable(ProcessFaultTests process_fault_tests.cpp ../include/platform.cpp)
  target_include_directories(ProcessFaultTests PRIVATE ../include)
  set_target_properties(ProcessFaultTests PROPERTIES CXX_STANDARD 20 CXX_MODULE_STD OFF CXX_SCAN_FOR_MODULES OFF)
  foreach(symbol IN ITEMS posix_spawn_file_actions_init posix_spawn_file_actions_addopen posix_spawnattr_init waitpid)
    target_link_options(ProcessFaultTests PRIVATE "LINKER:--wrap=${symbol}")
  endforeach()
  list(APPEND integration_categories faults process)
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
    if(category STREQUAL "discovery")
      set_tests_properties(integration.${test_id} PROPERTIES ENVIRONMENT
        "ANNOTEST_CMAKE=${CMAKE_COMMAND};ANNOTEST_COMPILER=${CMAKE_CXX_COMPILER}")
    endif()
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

add_executable(DiscoveryFixture discovery_fixture.cpp)
set_target_properties(DiscoveryFixture PROPERTIES CXX_STANDARD 20 CXX_MODULE_STD OFF CXX_SCAN_FOR_MODULES OFF)

# Require our annotation diagnostic, rather than any compiler failure.
function(expect_invalid_annotation name expected)
  add_executable(Invalid_${name} EXCLUDE_FROM_ALL compile_fail/${name}.cpp)
  target_link_libraries(Invalid_${name} PRIVATE annotest)
  add_test(NAME compile_fail.${name}
    COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${CMAKE_BINARY_DIR} -DTARGET=Invalid_${name}
      "-DEXPECTED=${expected}" -P ${CMAKE_CURRENT_SOURCE_DIR}/expect_compile_failure.cmake)
  set_tests_properties(compile_fail.${name} PROPERTIES RUN_SERIAL TRUE TIMEOUT 100)
endfunction()
expect_invalid_annotation(required_arguments "Test has required arguments without parameterization")
expect_invalid_annotation(private_test "Annotated test and lifecycle methods must be public")
expect_invalid_annotation(protected_hook "Annotated test and lifecycle methods must be public")
expect_invalid_annotation(hook_arguments "Lifecycle hooks must not have required arguments")
expect_invalid_annotation(deleted_test "Annotated methods must not be deleted")
expect_invalid_annotation(empty_parameters "Test parameterization must contain at least one case")
expect_invalid_annotation(empty_template_parameters "Test parameterization must contain at least one case")
expect_invalid_annotation(empty_dual_parameters "Test parameterization must contain at least one case")
expect_invalid_annotation(duplicate_names "Duplicate test name")
expect_invalid_annotation(expanded_duplicate_names "Duplicate expanded test name")
expect_invalid_annotation(newline_name "Test names must not contain tabs or line breaks")

foreach(invalid IN ITEMS generated_duplicate_names generated_empty_name)
  add_executable(Invalid_${invalid} EXCLUDE_FROM_ALL compile_fail/${invalid}.cpp)
  target_link_libraries(Invalid_${invalid} PRIVATE annotest)
  if(invalid STREQUAL "generated_duplicate_names")
    set(expected "Duplicate test name")
  else()
    set(expected "Test names must not be empty")
  endif()
  add_test(NAME compile_fail.${invalid}
    COMMAND ${CMAKE_COMMAND} -DBUILD_DIR=${CMAKE_BINARY_DIR} -DTARGET=Invalid_${invalid} "-DEXPECTED=${expected}"
      -P ${CMAKE_CURRENT_SOURCE_DIR}/expect_compile_failure.cmake)
  set_tests_properties(compile_fail.${invalid} PROPERTIES RUN_SERIAL TRUE TIMEOUT 100)
endforeach()

expect_invalid_annotation(zero_repetitions "Test repetitions must be between 1 and 1000000")
expect_invalid_annotation(excessive_repetitions "Test repetitions must be between 1 and 1000000")
expect_invalid_annotation(invalid_condition "Execution conditions must return bool")

expect_invalid_annotation(nonstatic_before_all "Suite hooks must be static methods")
expect_invalid_annotation(suite_hook_arguments "Lifecycle hooks must not have required arguments")
expect_invalid_annotation(duplicate_before_all "Suite hooks cannot share test or lifecycle annotations")
expect_invalid_annotation(suite_hook_then_test "Cannot have a `test` annotation on a lifecycle")
expect_invalid_annotation(test_then_suite_hook "Suite hooks cannot share test or lifecycle annotations")
expect_invalid_annotation(suite_hook_then_each "Suite hooks cannot share test or lifecycle annotations")
expect_invalid_annotation(each_then_suite_hook "Suite hooks cannot share test or lifecycle annotations")
expect_invalid_annotation(private_suite_hook "Annotated test and lifecycle methods must be public")
