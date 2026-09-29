cmake_minimum_required(VERSION 3.24)

project(Azer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AZER_ENABLE_DOCKING "Enable ImGui docking for editor-style applications" OFF)

add_subdirectory(vendor)
add_subdirectory(src)

add_library(AzerCore INTERFACE)

target_link_libraries(AzerCore INTERFACE Azer vendor)