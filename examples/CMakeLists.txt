add_executable(example1_basic example01_basic.cpp)
target_link_libraries(example1_basic PRIVATE annotest)

add_executable(example2_lifecycle example02_lifecycle.cpp)
target_link_libraries(example2_lifecycle PRIVATE annotest)

add_executable(example3_parameterized example03_parameterized.cpp)
target_link_libraries(example3_parameterized PRIVATE annotest)

add_executable(example4_os_annotations example04_os_annotations.cpp)
target_link_libraries(example4_os_annotations PRIVATE annotest)

add_executable(example5_death_tests example05_death_tests.cpp)
target_link_libraries(example5_death_tests PRIVATE annotest)

add_executable(example6_contracts example06_contracts.cpp)
target_link_libraries(example6_contracts PRIVATE annotest)

add_executable(example7_full_suite example07_full_suite.cpp)
target_link_libraries(example7_full_suite PRIVATE annotest)

add_executable(example8_typed example08_typed.cpp)
target_link_libraries(example8_typed PRIVATE annotest)

add_executable(example9_dual_parameters example09_dual_parameters.cpp)
target_link_libraries(example9_dual_parameters PRIVATE annotest)

add_executable(example10_reporting example10_reporting.cpp)
target_link_libraries(example10_reporting PRIVATE annotest)

target_compile_options(example6_contracts PRIVATE
  -fcontracts -fcontract-evaluation-semantic=observe)

if(BUILD_TESTS)
  foreach(example IN ITEMS example1_basic example2_lifecycle example3_parameterized
      example4_os_annotations example5_death_tests example6_contracts
      example7_full_suite example8_typed example9_dual_parameters example10_reporting)
    add_test(NAME examples.${example} COMMAND $<TARGET_FILE:${example}>)
    set_tests_properties(examples.${example} PROPERTIES TIMEOUT 30 LABELS examples)
  endforeach()
endif()

add_executable(example11_named_suites example11_named_suites.cpp)
target_link_libraries(example11_named_suites PRIVATE annotest)
if(BUILD_TESTS)
  add_test(NAME examples.example11_named_suites COMMAND $<TARGET_FILE:example11_named_suites>)
  set_tests_properties(examples.example11_named_suites PROPERTIES TIMEOUT 30 LABELS examples)
endif()

foreach(example IN ITEMS 12_matchers_and_threads 13_suite_resources 14_runtime_sources 15_execution)
  add_executable(example${example} example${example}.cpp)
  target_link_libraries(example${example} PRIVATE annotest)
  if(BUILD_TESTS)
    add_test(NAME examples.example${example} COMMAND $<TARGET_FILE:example${example}>)
    set_tests_properties(examples.example${example} PROPERTIES TIMEOUT 30 LABELS examples)
  endif()
endforeach()
if(BUILD_TESTS)
  add_test(NAME examples.parallel_execution
    COMMAND $<TARGET_FILE:example15_execution> --jobs 2 --timeout 5 --repeat 2)
  set_tests_properties(examples.parallel_execution PROPERTIES TIMEOUT 30 LABELS examples)
endif()
