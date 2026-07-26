add_library(Azer

        # --- Precompiled Header ---
        azpch.cpp

        # --- Core ---
        base/Application.cpp
        base/DeltaTime.cpp
        base/GameObject.cpp
        base/Input.cpp
        base/LayerStack.cpp
        base/Logger.cpp
        base/Scene.cpp
        base/SplashLayer.cpp
        base/Variant.cpp
        base/Window.cpp

        # --- Events ---
        base/event/Event.cpp

        # --- File System ---
        base/file_system/FileSystem.cpp

        # --- ImGui ---
        base/ImGuiLayer.cpp

        # --- Animation ---
        base/animation/AnimationPlayer.cpp

        # --- Serialization ---
        base/SceneSerializer.cpp

        # --- Reflection ---
        base/reflection/PropertyAccessor.cpp

        # --- ECS ---
        ecs/World.cpp
        ecs/SystemManager.cpp
        ecs/RenderSystem.cpp
        ecs/PhysicsSystem.cpp
        ecs/ECSLayer.cpp
        ecs/GameObjectWrapper.cpp
        ecs/ECSScene.cpp
        ecs/ECSSceneSerializer.cpp

        # --- Renderer (abstract) ---
        renderer/Framebuffer.cpp
        renderer/Model.cpp
        renderer/Renderer.cpp
        renderer/RendererAPI.cpp
        renderer/Texture.cpp
        renderer/StbImage.cpp

        # --- Backend: SDL3 Renderer (2D) ---
        backends/SDL3Renderer/SDL3Renderer.cpp
        backends/SDL3Renderer/SDL3Texture.cpp
        backends/SDL3Renderer/SDL3Framebuffer.cpp

        # --- Backend: SDL3 GPU Renderer (3D) ---
        backends/SDL3GPURenderer/SDL3GPURenderer.cpp
        backends/SDL3GPURenderer/SDL3GPURendererSupport.cpp
        backends/SDL3GPURenderer/GPUTexture.cpp
        backends/SDL3GPURenderer/SDL3GPUFramebuffer.cpp

        # --- Backend: SDL3 Window ---
        backends/SDL3Window/SDL3Window.cpp
        base/file_system/FileSystem.cpp
        base/file_system/FileSystem.h

        # --- Backend: Vulkan ---
        backends/Vulkan/VulkanRendererContext.cpp
        backends/Vulkan/VulkanFrameBuffer.cpp
        backends/Vulkan/VulkanRenderer.cpp
        backends/Vulkan/vk_mem_alloc.cpp
        backends/Vulkan/VulkanCommandBuffer.cpp
)

target_precompile_headers(Azer PRIVATE azpch.h)

target_include_directories(Azer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/base
        ${CMAKE_CURRENT_SOURCE_DIR}/base/event
        ${CMAKE_CURRENT_SOURCE_DIR}/base/animation
        ${CMAKE_CURRENT_SOURCE_DIR}/base/reflection
        ${CMAKE_CURRENT_SOURCE_DIR}/base/file_system
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/ecs
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3GPURenderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/SDL3Window
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/Vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/entt/single_include
)

target_link_libraries(Azer PRIVATE vendor)

target_compile_definitions(Azer PRIVATE _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL AZ_DEBUG)

if(AZER_ENABLE_DOCKING)
    target_compile_definitions(Azer PUBLIC AZER_ENABLE_DOCKING)
endif()
