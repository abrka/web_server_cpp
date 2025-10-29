cmake_minimum_required(VERSION 3.10)
project(web_app_server_proj)

add_executable(web_app_server "src/main.cpp")

target_include_directories(web_app_server PRIVATE "src" "src/networking")

set_target_properties(web_app_server PROPERTIES CXX_STANDARD 20)
target_compile_options(web_app_server PRIVATE -pedantic -Wall -Wextra -Wconversion -fsanitize=address,leak,undefined)
target_link_options(web_app_server PRIVATE -fsanitize=address,leak,undefined)


add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(web_app_server copy_assets)