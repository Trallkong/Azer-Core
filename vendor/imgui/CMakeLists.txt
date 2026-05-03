add_library(imgui
        imgui.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui_demo.cpp
        backends/imgui_impl_sdl3.cpp
        backends/imgui_impl_sdlrenderer3.cpp
        backends/imgui_impl_sdlgpu3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
)
target_link_libraries(imgui PRIVATE SDL3::SDL3)