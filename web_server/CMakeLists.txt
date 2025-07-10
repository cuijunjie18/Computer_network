# 为clangd产生配置文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.10)
project(web_server)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    web_server
    ${PROJECT_SOURCE_DIR}/src/web_server.cpp
)

add_executable(
    web_client
    ${PROJECT_SOURCE_DIR}/src/web_client.cpp
)

add_executable(
    debug
    ${PROJECT_SOURCE_DIR}/src/debug.cpp
)

# add_executable(
#     check
#     ${PROJECT_SOURCE_DIR}/src/check.cpp
# )