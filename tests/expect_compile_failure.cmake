execute_process(COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "${TARGET}"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 90)
if(result EQUAL 0)
  message(FATAL_ERROR "Invalid annotation unexpectedly compiled: ${TARGET}")
endif()
# Require a diagnostic from annotation validation, rather than treating any
# compiler, dependency, linker, or infrastructure failure as a successful test.
set(diagnostic "${output}\n${error}")
if(NOT diagnostic MATCHES "get_tests" OR NOT diagnostic MATCHES "${EXPECTED}")
  message(FATAL_ERROR "Expected annotation-validation diagnostic for ${TARGET}:\n${diagnostic}")
endif()
