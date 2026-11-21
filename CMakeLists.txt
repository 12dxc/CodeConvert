cmake_minimum_required(VERSION 3.16)
project(CodeConvert VERSION 1.1.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源码为 UTF-8 无 BOM 且含中文字面量，MSVC 需显式指定 /utf-8 才不会乱码
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 查找 Qt6（qt_standard_project_setup 需要 Qt 6.3+）
find_package(Qt6 6.3 REQUIRED COMPONENTS Widgets Core Gui Core5Compat)
qt_standard_project_setup()

# 从 project(VERSION) 生成版本头文件（main.cpp 引用，版本号单一来源）
configure_file(version.h.in version.h @ONLY)

# 查找 uchardet；未找到时自动下载源码构建，避免手工安装依赖
find_path(UCHARDET_INCLUDE_DIR NAMES uchardet/uchardet.h)
find_library(UCHARDET_LIBRARY NAMES uchardet libuchardet)

if(UCHARDET_INCLUDE_DIR AND UCHARDET_LIBRARY AND EXISTS "${UCHARDET_LIBRARY}")
    message(STATUS "使用系统 uchardet: ${UCHARDET_LIBRARY}")
    add_library(uchardet_ext UNKNOWN IMPORTED)
    set_target_properties(uchardet_ext PROPERTIES
        IMPORTED_LOCATION "${UCHARDET_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${UCHARDET_INCLUDE_DIR}")
    set(CODECONVERT_UCHARDET_LIB uchardet_ext)
else()
    message(STATUS "未找到系统 uchardet，将通过 FetchContent 下载源码构建")
    include(FetchContent)
    FetchContent_Declare(uchardet
        GIT_REPOSITORY https://gitlab.freedesktop.org/uchardet/uchardet.git
        GIT_TAG v0.0.8)
    # uchardet v0.0.8 的 CMakeLists.txt 声明的最低版本低于 3.5，
    # 新版 CMake 已移除兼容性，需临时放开限制
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    FetchContent_MakeAvailable(uchardet)
    unset(CMAKE_POLICY_VERSION_MINIMUM)

    # uchardet 的库目标名为 libuchardet（uchardet 是它的命令行工具）；
    # 源码树头文件是平铺的（src/uchardet.h），生成 shim 目录保持
    # #include <uchardet/uchardet.h> 在两种来源下写法一致
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/uchardet-shim/uchardet")
    file(COPY "${uchardet_SOURCE_DIR}/src/uchardet.h"
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/uchardet-shim/uchardet")
    add_library(uchardet_fetch INTERFACE)
    target_link_libraries(uchardet_fetch INTERFACE libuchardet)
    target_include_directories(uchardet_fetch INTERFACE
        "${CMAKE_CURRENT_BINARY_DIR}/uchardet-shim")
    set(CODECONVERT_UCHARDET_LIB uchardet_fetch)
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/worker.cpp
    src/encodedetector.cpp
)

set(HEADERS
    src/mainwindow.h
    src/worker.h
    src/encodedetector.h
)

set(UI_FILES
    src/mainwindow.ui
)

# 创建可执行文件（WIN32 表示 Windows GUI 程序，不显示控制台窗口）
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${UI_FILES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Core5Compat
    ${CODECONVERT_UCHARDET_LIB}
)

# 包含目录（含生成的 version.h）
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 输出信息
message(STATUS "Qt 版本: ${Qt6_VERSION}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
