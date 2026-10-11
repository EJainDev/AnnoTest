function(annotest_discover_tests TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 discovery "" "DISCOVERY_TIMEOUT" "")
  if(discovery_UNPARSED_ARGUMENTS OR discovery_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR "Invalid annotest_discover_tests arguments for ${TARGET}")
  endif()
  if(NOT TARGET "${TARGET}")
    message(FATAL_ERROR "AnnoTest discovery target does not exist: ${TARGET}")
  endif()
  get_target_property(target_type "${TARGET}" TYPE)
  get_target_property(imported "${TARGET}" IMPORTED)
  get_target_property(alias "${TARGET}" ALIASED_TARGET)
  if(NOT target_type STREQUAL "EXECUTABLE" OR imported OR alias)
    message(FATAL_ERROR "AnnoTest discovery requires a locally built executable target: ${TARGET}")
  endif()
  if(NOT DEFINED discovery_DISCOVERY_TIMEOUT)
    set(discovery_DISCOVERY_TIMEOUT 30)
  endif()
  if(NOT discovery_DISCOVERY_TIMEOUT MATCHES "^[1-9][0-9]*$")
    message(FATAL_ERROR "DISCOVERY_TIMEOUT must be a positive integer in seconds")
  endif()

  # Keep configurations separate and report a useful CTest error before a build.
  set(manifest "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_$<CONFIG>_discovered_tests.cmake")
  set(guard "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_$<CONFIG>_discovery_guard.cmake")
  file(GENERATE OUTPUT "${guard}" CONTENT
"if(NOT EXISTS [==[$<TARGET_FILE:${TARGET}>]==])
  message(FATAL_ERROR [==[AnnoTest executable is missing. Build target ${TARGET} before running CTest.]==])
endif()
if(NOT EXISTS [==[${manifest}]==])
  message(FATAL_ERROR [==[AnnoTest discovery manifest is missing. Build target ${TARGET} and resolve its discovery errors before running CTest.]==])
endif()
include([==[${manifest}]==])
")
  # TEST_INCLUDE_FILES does not evaluate generator expressions. Dispatch at
  # CTest time for multi-config generators; single-config builds know their type.
  set(dispatch "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_discovery_guard.cmake")
  get_property(multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(multi_config)
    file(WRITE "${dispatch}"
"if(NOT DEFINED CTEST_CONFIGURATION_TYPE OR CTEST_CONFIGURATION_TYPE STREQUAL \"\")
  message(FATAL_ERROR \"AnnoTest discovery requires a configuration. Use ctest -C Debug or -C Release.\")
endif()
set(_annotest_guard_base [==[${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_]==])
string(CONCAT _annotest_guard \"\${_annotest_guard_base}\" \"\${CTEST_CONFIGURATION_TYPE}\" \"_discovery_guard.cmake\")
include(\"\${_annotest_guard}\")
")
  else()
    file(WRITE "${dispatch}"
      "include([==[${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_${CMAKE_BUILD_TYPE}_discovery_guard.cmake]==])\n")
  endif()
  set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${dispatch}")
  # Re-run discovery when the installed or source discovery script changes.
  set(script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/DiscoverTests.cmake")
  set_property(TARGET "${TARGET}" APPEND PROPERTY LINK_DEPENDS "${script}")
  add_custom_command(TARGET "${TARGET}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} "-DTEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
      "-DOUTPUT_FILE=${manifest}" "-DDISCOVERY_TIMEOUT=${discovery_DISCOVERY_TIMEOUT}"
      -P "${script}"
    COMMENT "Discovering AnnoTest cases for ${TARGET}"
    VERBATIM)
endfunction()
