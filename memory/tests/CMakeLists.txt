add_executable(MemoryTestsExecutable Memory.cpp)
target_link_libraries(MemoryTestsExecutable PRIVATE cpputils_memory
                                                    cpputils_testing)

add_test(NAME MemoryTests COMMAND MemoryTestsExecutable)

set_tests_properties(MemoryTests PROPERTIES FAIL_REGULAR_EXPRESSION "failed")
