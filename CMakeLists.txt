cmake_minimum_required(VERSION 3.16)
project(TimeTravelDebugger VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 尝试查找QScintilla，如果找不到则使用备用方案
find_package(QScintilla QUIET)

# 配置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# 设置可执行文件
add_executable(TimeTravelDebugger
    src/main.cpp
    src/mainwindow.cpp
    src/editorwidget.cpp
    src/variablewidget.cpp
    src/consolewidget.cpp
    src/controlwidget.cpp
    src/interpreter/interpreter.cpp
    src/interpreter/parser.cpp
    src/interpreter/ast.cpp
    src/interpreter/value.cpp
    src/interpreter/programstate.cpp
    src/interpreter/tokentype.cpp
    src/interpreter/tokenizer.cpp
    src/interpreter/executioncontext.cpp
)

# 设置目标属性
target_include_directories(TimeTravelDebugger PRIVATE src)
target_link_libraries(TimeTravelDebugger PRIVATE Qt6::Core Qt6::Widgets)

# 如果找到QScintilla，使用它
if(QScintilla_FOUND)
    target_link_libraries(TimeTravelDebugger PRIVATE QScintilla)
    target_compile_definitions(TimeTravelDebugger PRIVATE USE_QSCINTILLA)
else()
    message(STATUS "QScintilla not found, using QPlainTextEdit as fallback")
endif()

# 复制资源文件（如果需要）
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/res)
    file(GLOB_RECURSE RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/res/*")
    target_sources(TimeTravelDebugger PRIVATE ${RESOURCES})
endif()