cmake_minimum_required(VERSION 3.16)
project(CodeConvert LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Core5Compat)
qt_standard_project_setup()


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
    uchardet
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 输出信息
message(STATUS "Qt 版本: ${Qt6_VERSION}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
