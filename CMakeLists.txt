cmake_minimum_required(VERSION 3.16)
project(CodeConvert LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Core5Compat)

# 查找 uchardet（优先本地，找不到自动下载）
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(UCHARDET QUIET libuchardet)
endif()

if(NOT UCHARDET_FOUND)
    find_path(UCHARDET_INCLUDE_DIRS
        NAMES uchardet/uchardet.h
        PATHS ${CMAKE_PREFIX_PATH}/include
    )
    find_library(UCHARDET_LIBRARIES
        NAMES uchardet
        PATHS ${CMAKE_PREFIX_PATH}/lib
    )
    if(UCHARDET_INCLUDE_DIRS AND UCHARDET_LIBRARIES)
        set(UCHARDET_FOUND TRUE)
        message(STATUS "找到本地 uchardet: ${UCHARDET_LIBRARIES}")
    endif()
endif()

if(NOT UCHARDET_FOUND)
    message(STATUS "本地未找到 uchardet，自动下载...")
    include(FetchContent)
    FetchContent_Declare(
        uchardet
        GIT_REPOSITORY https://github.com/nicstange/uchardet.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(uchardet)
    set(UCHARDET_LIBRARIES uchardet)
    set(UCHARDET_INCLUDE_DIRS "")
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

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Core5Compat
    ${UCHARDET_LIBRARIES}
)

# 包含目录
if(UCHARDET_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${UCHARDET_INCLUDE_DIRS}
    )
else()
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
endif()

# 输出信息
message(STATUS "Qt 版本: ${Qt6_VERSION}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
