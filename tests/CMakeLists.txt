add_executable(AssertionTests assertion_tests.cpp)
target_link_libraries(AssertionTests PRIVATE annotest)

annotest_discover_tests(AssertionTests)

add_executable(TupleTests tuple_tests.cpp)
target_link_libraries(TupleTests PRIVATE annotest)

annotest_discover_tests(TupleTests)
