set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10)
project(SMTP_Lab)

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 头文件和包
find_package(OpenSSL REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OPENSSL_INCLUDE_DIR})

# 生成可执行文件
set(
    SMTP_client_src
    ${PROJECT_SOURCE_DIR}/src/SMTP_client.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
)
add_executable(
    SMTP_client
    ${SMTP_client_src}
)
target_link_libraries(
    SMTP_client
    ${OPENSSL_LIBRARIES}
)

# 生成Debug可执行文件

# set(
#     Debug_src
#     ${PROJECT_SOURCE_DIR}/src/debug.cpp
#     ${PROJECT_SOURCE_DIR}/src/utils.cpp
# )

# message(${Debug_src})

# add_executable(
#     debug
#     ${Debug_src}
# )