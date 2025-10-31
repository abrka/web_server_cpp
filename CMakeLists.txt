cmake_minimum_required(VERSION 3.10)
project(web_server_proj)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(networking_lib INTERFACE)

target_include_directories(networking_lib INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/networking)

set_target_properties(networking_lib PROPERTIES CXX_STANDARD 20)
target_compile_options(networking_lib INTERFACE -pedantic -Wall -Wextra -Wconversion -fsanitize=address,leak,undefined)
target_link_options(networking_lib INTERFACE -fsanitize=address,leak,undefined)

add_subdirectory(example)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(external/googletest)

enable_testing()
add_subdirectory(tests/http)