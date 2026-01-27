cmake_minimum_required(VERSION 3.20)

# 项目名称
project(TinyBLE_Windows_Sim C)

set(CMAKE_C_STANDARD 99)

# =========================================================
# 1. 【关键】定义根目录 (向上跳两级)
# =========================================================
# CMAKE_CURRENT_SOURCE_DIR 现在是 .../examples/windows
# 我们需要让 PROJ_ROOT 指向 .../TinyStack_ETR
get_filename_component(PROJ_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." REALPATH)

# 打印一下路径，确保找对了 (调试用)
message(STATUS "Project Root is: ${PROJ_ROOT}")

# =========================================================
# 2. 基于根目录定义路径
# =========================================================
set(SRC_DIR       "${PROJ_ROOT}/source")
set(UTILS_DIR     "${PROJ_ROOT}/utils")
set(PLATFORM_DIR  "${PROJ_ROOT}/platform")
set(LIB_DIR       "${PROJ_ROOT}/Third_party")

# =========================================================
# 3. 头文件搜索路径 (Include Directories)
# =========================================================
include_directories(
        # 核心栈
        ${SRC_DIR}/stack

        # 服务
        ${SRC_DIR}/Server/Battery
        ${SRC_DIR}/Server/Heart_Rate
        ${SRC_DIR}/Server/Hid_mouse
        ${SRC_DIR}/Server/Name
        ${SRC_DIR}/Server/rdtss
        ${SRC_DIR}/Server

        # 工具
        ${UTILS_DIR}/fifo
        ${UTILS_DIR}/timer
        ${UTILS_DIR}/btsnoop

        # 平台接口 (Windows)
        ${PLATFORM_DIR}/windows

        # 第三方库
        ${LIB_DIR}/crypto/include
#        ${LIB_DIR}/sbc/sbc


        #配置文件
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# =========================================================
# 4. 搜集源文件 (Source Files)
# =========================================================
# 4.1 核心代码
file(GLOB_RECURSE STACK_SRCS  "${SRC_DIR}/stack/*.c")
file(GLOB_RECURSE SERVER_SRCS "${SRC_DIR}/Server/*.c")
file(GLOB_RECURSE UTILS_SRCS  "${UTILS_DIR}/*.c")

# 4.2 平台代码 (只取 Windows)
file(GLOB_RECURSE PLATFORM_SRCS "${PLATFORM_DIR}/windows/*.c")

# 4.3 第三方库
file(GLOB_RECURSE CRYPTO_SRCS "${LIB_DIR}/crypto/source/*.c")
file(GLOB SBC_SRCS "${LIB_DIR}/sbc/sbc/*.c")

# =========================================================
# 5. 生成可执行文件
# =========================================================

add_executable(TinyBLE_Sim
        # 【改动】因为 CMakeLists 就在本目录，所以直接写 main.c
        main.c

        # 其他源码
        ${STACK_SRCS}
        ${SERVER_SRCS}
        ${UTILS_SRCS}
        ${PLATFORM_SRCS}
        ${CRYPTO_SRCS}
        ${SBC_SRCS}
        config.h
)

add_definitions(-DPLATFORM_WINDOWS)

if(UNIX OR MINGW)
    target_link_libraries(TinyBLE_Sim m pthread)
endif()