execute_process(COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target HiddenException
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 90)
if(result EQUAL 0)
  message(FATAL_ERROR "assertion_error unexpectedly became accessible to importing code")
endif()
set(diagnostic "${output}\n${error}")
if(NOT diagnostic MATCHES "assertion_error" OR NOT diagnostic MATCHES "does not name a type|is not a member")
  message(FATAL_ERROR "Expected an inaccessible assertion_error diagnostic:\n${diagnostic}")
endif()
