add_executable(AnnoTest_Tests test.cpp)
target_link_libraries(AnnoTest_Tests PRIVATE annotest)

annotest_discover_tests(AnnoTest_Tests)
