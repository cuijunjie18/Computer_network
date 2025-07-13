set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.10)
project(SMTP_Lab)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    test_server
    ${PROJECT_SOURCE_DIR}/src/test_server.cpp
)

add_executable(
    SMTP_client
    ${PROJECT_SOURCE_DIR}/src/SMTP_client.cpp
)

add_executable(
    debug
    ${PROJECT_SOURCE_DIR}/src/debug.cpp
)