cmake_minimum_required(VERSION 3.13)

project(GLFW)

add_library(GLFW INTERFACE)

target_include_directories(GLFW INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GLFW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib-mingw-w64/libglfw3.a")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(GLFW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib-vc2026/glfw3.lib")
else ()
    message(FATAL_ERROR "不支持的编译器: ${CMAKE_CXX_COMPILER_ID}")
endif ()

if(NOT EXISTS "${GLFW_LIB_PATH}")
    message(FATAL_ERROR "找不到 GLFW 预编译库: ${GLFW_LIB_PATH}")
endif()

# 直接给绝对路径：MinGW 下 -lglfw3 找不到 libglfw3.a（目录里是 libglfw3dll.a），
# 绝对不能走 target_link_directories + -l 的组合。
target_link_libraries(GLFW INTERFACE "${GLFW_LIB_PATH}")

if (WIN32)
    target_link_libraries(GLFW INTERFACE opengl32 gdi32 user32 shell32)
endif ()