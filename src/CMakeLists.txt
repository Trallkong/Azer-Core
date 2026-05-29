add_library(Azer
        azpch.cpp
        base/DeltaTime.cpp
        base/LayerStack.cpp
        base/Application.cpp
        base/event/Event.cpp
        base/ImGuiLayer.cpp
        base/Window.cpp
        base/Logger.cpp
        base/SplashLayer.cpp
        base/Input.cpp
        base/Variant.cpp
        base/Random.h
        base/Variant.h
        base/animation/AnimationPlayer.cpp
        base/animation/AnimationPlayer.h
        base/animation/Animation.h
        base/reflection/PropertyAccessor.cpp
        base/reflection/PropertyAccessor.h

        renderer/Model.cpp
        renderer/Texture.cpp
        renderer/StbImage.cpp

        backends/SDL3Renderer/SDL3Renderer.cpp
        backends/SDL3Renderer/SDL3Texture.cpp
        backends/SDL3GPURenderer/SDL3GPURenderer.cpp
        backends/SDL3GPURenderer/GPUTexture.cpp

        backends/SDL3Window/SDL3Window.cpp
        backends/SDL3GPURenderer/SDL3GPURendererSupport.cpp
)

target_precompile_headers(Azer PRIVATE azpch.h)

target_include_directories(Azer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/base
        ${CMAKE_CURRENT_SOURCE_DIR}/base/event
        ${CMAKE_CURRENT_SOURCE_DIR}/base/animation
        ${CMAKE_CURRENT_SOURCE_DIR}/base/reflection
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3GPURenderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3Window
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders
)

target_link_libraries(Azer PRIVATE vendor)

target_compile_definitions(Azer PRIVATE _CRT_SECURE_NO_WARNINGS)
