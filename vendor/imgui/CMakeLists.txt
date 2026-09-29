add_library(imgui
        imgui.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui_demo.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_vulkan.cpp
)

find_package(Vulkan REQUIRED)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
)
# imgui_impl_glfw.cpp 需要 GLFW 的头文件，GLFW 是 INTERFACE 目标，
# 不链接它就拿不到 include 目录（vendor 里链接 GLFW 只对最终可执行文件生效，
# 不会传播到 imgui 自身的编译）。
target_link_libraries(imgui PRIVATE GLFW Vulkan::Vulkan)