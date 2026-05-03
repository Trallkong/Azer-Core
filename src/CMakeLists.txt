add_library(Azer
        base/DeltaTime.cpp
        base/LayerStack.cpp
        base/Application.cpp
        base/Event.cpp
        base/ImGuiLayer.cpp
        base/Window.cpp
        base/Logger.cpp

        backends/SDL3Renderer/SDL3Renderer.cpp
        backends/SDL3GPURenderer/SDL3GPURenderer.cpp
        renderer/Renderer2D.cpp

        azpch.cpp
        renderer/Texture.h
        backends/SDL3Renderer/SDL3Texture.h
        backends/SDL3GPURenderer/GPUTexture.h
        base/SplashLayer.h
        base/SplashLayer.cpp
)

target_precompile_headers(Azer PRIVATE azpch.h)

target_include_directories(Azer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/base
        ${CMAKE_CURRENT_SOURCE_DIR}/base/node
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3GPURenderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3Renderer
)

target_link_libraries(Azer PRIVATE vendor)

