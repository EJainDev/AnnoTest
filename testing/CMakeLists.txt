add_library(cpputils_testing SHARED)
target_sources(
  cpputils_testing
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES
         "Tuple.cxx"
         "Exceptions.cxx"
         "Asserts.cxx"
         "Expects.cxx"
         "DeathTest.cxx"
         "Test.cxx"
         "Posix.cxx")
target_link_libraries(cpputils_testing PUBLIC cpputils_refl)

add_library(cpputils::testing ALIAS cpputils_testing)

add_dependencies(cpputils cpputils_testing)

install(
  TARGETS cpputils_testing
  EXPORT cpputils_targets
  FILE_SET modules DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpputils/testing
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

function(cpputils_discover_tests TARGET)
  # Path where the generated test list will live
  set(CTEST_DISCOVERY_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake")

  # Run the parser script every time the binary is rebuilt
  add_custom_command(
    TARGET ${TARGET}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>" -D
      "CTEST_FILE=${CTEST_DISCOVERY_FILE}" -P
      "${CMAKE_SOURCE_DIR}/cmake/CppUtilsDiscoverTests.cmake"
    COMMENT "Parsing tests from ${TARGET} output..."
    VERBATIM)

  # Include the generated tests in CTest
  set_property(
    DIRECTORY
    APPEND
    PROPERTY TEST_INCLUDE_FILES "${CTEST_DISCOVERY_FILE}")
endfunction()

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
