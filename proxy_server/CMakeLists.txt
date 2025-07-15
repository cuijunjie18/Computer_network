# 为clangd产生配置文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.10)
project(proxy_server)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Main
set(
    proxy_server_src
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/proxy_server.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
)
add_executable(
    main
    ${proxy_server_src}
)

# debug
add_executable(
    debug
    ${PROJECT_SOURCE_DIR}/src/debug.cpp
)