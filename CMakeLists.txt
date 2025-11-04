cmake_minimum_required(VERSION 3.10)
project(web_server_proj)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


file(GLOB NETWORKING_LIB_SOURCES "src/*.hpp")
add_library(networking_lib INTERFACE ${NETWORKING_LIB_SOURCES})
add_library(networking_lib::networking_lib ALIAS networking_lib)
target_include_directories(networking_lib INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/networking)


set_target_properties(networking_lib PROPERTIES CXX_STANDARD 20)
target_compile_options(networking_lib INTERFACE -pedantic -Wall -Wextra -Wconversion -fstack-protector-all -fsanitize=address,leak,undefined)
target_link_options(networking_lib INTERFACE -fsanitize=address,leak,undefined)

add_subdirectory(example)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)
add_subdirectory(external/googletest)

enable_testing()
add_subdirectory(tests/http)

include(CPack)