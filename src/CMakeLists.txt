add_library(Azer

        # --- Precompiled Header ---
        azpch.cpp


        # --- Core ---
        base/Application.cpp
        base/DeltaTime.cpp
        base/GameObject.cpp
        base/LayerStack.cpp
        base/Logger.cpp
        base/Scene.cpp
        base/SplashLayer.cpp
        base/Variant.cpp
        base/Window.cpp
        base/EntryPoint.h
        base/Type.h
        base/Input.h

        base/file_system/FileSystem.cpp
        base/file_system/FileSystem.h

        # --- Events ---
        base/event/Event.h
        base/event/KeyEvent.h
        base/event/MouseEvent.h
        base/event/WindowEvent.h

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

        # --- Renderer (abstract) ---
        renderer/Framebuffer.cpp
        renderer/IndexBuffer.cpp
        renderer/Model.cpp
        renderer/RenderCommand.cpp
        renderer/Renderer.cpp
        renderer/Renderer2D.cpp
        renderer/Renderer3D.cpp
        renderer/RendererAPI.cpp
        renderer/Shader.cpp
        renderer/Texture.cpp
        renderer/VertexBuffer.cpp
        renderer/Mesh2D.h

        # --- Backend: WindowsWindow ---
        backends/WindowsWindow/WindowsWindow.h
        backends/WindowsWindow/WindowsWindow.cpp

        # --- Backend: Vulkan ---
        backends/Vulkan/VulkanContextManager.cpp
        backends/Vulkan/vk_mem_alloc.cpp
        backends/Vulkan/VulkanCommandBuffer.cpp
        backends/Vulkan/VulkanSwapchain.cpp

        # --- Backend: Vulkan Renderer ---
        backends/Vulkan/VulkanRenderer/VulkanRenderer.cpp
        backends/Vulkan/VulkanRenderer/VulkanFrameBuffer.cpp
        backends/Vulkan/VulkanRenderer/VulkanShader.cpp
        backends/Vulkan/VulkanRenderer/VulkanDescriptorAllocator.cpp
        backends/Vulkan/VulkanRenderer/VulkanDescriptorSet.cpp
        backends/Vulkan/VulkanRenderer/VulkanUniformBuffer.cpp
        backends/Vulkan/VulkanRenderer/VulkanVertexBuffer.cpp
        backends/Vulkan/VulkanRenderer/VulkanIndexBuffer.cpp
        backends/Vulkan/VulkanRenderer/VulkanStagingBuffer.cpp
        backends/Vulkan/VulkanRenderer/VulkanTexture.cpp

        # resources
        resources/SkyBox.cpp
        resources/node/Node.h
        resources/node/Node.cpp
        resources/node/Node2D.h
        resources/node/Node3D.h
        resources/node/NodeManager.h
        resources/node/NodeManager.cpp
        resources/node/node2d/Sprite2D.h
        resources/node/node2d/Sprite2D.cpp
        resources/node/node2d/ui/Button.h
        resources/node/node2d/ui/Button.cpp
        base/KeyCode.h
        base/Input.cpp

)

target_precompile_headers(Azer PRIVATE azpch.h)

target_include_directories(Azer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/base
        ${CMAKE_CURRENT_SOURCE_DIR}/base/event
        ${CMAKE_CURRENT_SOURCE_DIR}/base/animation
        ${CMAKE_CURRENT_SOURCE_DIR}/base/reflection
        ${CMAKE_CURRENT_SOURCE_DIR}/base/file_system
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/Vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/Vulkan/VulkanRenderer
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/WindowsWindow
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/node
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/node/node2d
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/node/node2d/ui
)

# PUBLIC：Azer 的公共头文件用到了 vendor 里的库，且 spdlog 的
# SPDLOG_COMPILED_LIB 定义必须传递给所有使用者，否则会重复定义。
target_link_libraries(Azer PUBLIC vendor)

target_compile_definitions(Azer PRIVATE _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL AZ_ENABLE_DEBUG)

# 资源根目录（Azer-Core/）：assets/shaders 等都相对于它解析。
# 用编译期常量注入，避免在源码里写死某台机器的绝对路径。
get_filename_component(AZER_ASSET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
target_compile_definitions(Azer PRIVATE AZER_ASSET_ROOT="${AZER_ASSET_ROOT}")

if(AZER_ENABLE_DOCKING)
    target_compile_definitions(Azer PUBLIC AZER_ENABLE_DOCKING)
endif()
