cmake_minimum_required(VERSION 3.22)
project(web_server_example_proj)

add_executable(web_server_example "main.cpp")

target_link_libraries(web_server_example PUBLIC networking_lib)


set_target_properties(web_server_example PROPERTIES CXX_STANDARD 20)
target_compile_options(web_server_example PRIVATE -pedantic -Wall -Wextra -Wconversion -fsanitize=address,leak,undefined)
target_link_options(web_server_example PRIVATE -fsanitize=address,leak,undefined)


add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(web_server_example copy_assets)