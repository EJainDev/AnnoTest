# CppUtilsDiscoverTests.cmake

# 1. Capture the test list from the binary
execute_process(
  COMMAND "${TEST_EXECUTABLE}" "--list"
  OUTPUT_VARIABLE TEST_LIST
  ERROR_VARIABLE TEST_ERR
  RESULT_VARIABLE res
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT res EQUAL 0)
  message(FATAL_ERROR "Failed to list tests from ${TEST_EXECUTABLE}")
endif()

# Fallback to stderr if stdout is empty
if(NOT TEST_LIST AND TEST_ERR)
  set(TEST_LIST "${TEST_ERR}")
endif()

# 1. Split output into lines
string(REGEX REPLACE "\r?\n" ";" LINES "${TEST_LIST}")

set(CTEST_FILE_CONTENT "")
set(CURRENT_SUITE "")

foreach(line ${LINES})
  # Check for SUITE: No leading spaces, ends with a dot Example: "MathSuite."
  if(line MATCHES "^([^ ]+)\\.$")
    set(CURRENT_SUITE "${CMAKE_MATCH_1}")

    # Check for TEST CASE: Exactly two leading spaces Example: "  Addition"
  elseif(line MATCHES "^  ([^ ]+)")
    set(test_case "${CMAKE_MATCH_1}")

    if(NOT CURRENT_SUITE STREQUAL "")
      # The name in CTest will be "SuiteName.TestCase"
      set(full_test_name "${CURRENT_SUITE}.${test_case}")

      # Add the test command
      string(
        APPEND
        CTEST_FILE_CONTENT
        "add_test(\"${full_test_name}\" \"${TEST_EXECUTABLE}\" \"--filter=${CURRENT_SUITE}.${test_case}\")\n"
      )
    endif()
  endif()
endforeach()

# 1. Write the generated commands to the .cmake file
file(WRITE "${CTEST_FILE}" "${CTEST_FILE_CONTENT}")
