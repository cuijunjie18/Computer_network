# 为clangd产生配置文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.10)
project(udp_pinger)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    udp_server
    ${PROJECT_SOURCE_DIR}/src/udp_server.cpp
)

add_executable(
    udp_pinger
    ${PROJECT_SOURCE_DIR}/src/udp_pinger.cpp
)

add_executable(
    demo
    ${PROJECT_SOURCE_DIR}/src/demo.cpp
)