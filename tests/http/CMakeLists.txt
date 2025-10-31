cmake_minimum_required(VERSION 3.22)
project(http_test_proj)

add_executable(http_test main.cpp)

target_link_libraries(http_test GTest::gtest_main networking_lib)

include(GoogleTest)
gtest_discover_tests(http_test)
