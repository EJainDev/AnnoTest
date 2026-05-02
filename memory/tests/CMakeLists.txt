add_executable(MemoryTestsExecutable Memory.cpp)
target_link_libraries(MemoryTestsExecutable PRIVATE cpputils_memory
                                                    cpputils_testing)

cpputils_discover_tests(MemoryTestsExecutable)
