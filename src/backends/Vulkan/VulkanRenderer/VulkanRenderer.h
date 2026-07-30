#pragma once

#include "Base.h"
#include "Renderer.h"

#include "VulkanContextManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicPipeline.h"
#include "VulkanUniformBuffer.h"

namespace Azer {

    class VulkanVertexBuffer;
    class VulkanIndexBuffer;

    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer() = default;
        virtual ~VulkanRenderer() override;

        bool Initialize(Window* window) override;
        void Shutdown() override;

        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void SetCamera(Camera& camera) override;
        void ResetRenderState() override;
        void SetRenderTarget(Framebuffer* target) override;  // nullptr = swapchain
        void Resize(uint32_t width, uint32_t height) override;
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override;

        void DrawQuad(const Transform2D& transform, float alpha = 1.0f) override;
        void DrawColorQuad(const Transform2D& transform, const glm::vec4& color) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const Transform2D& transform, float alpha = 1.0f) override;

        void DrawCube(const Transform3D& transform) override;
        void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha = 1.0f) override;
        void DrawSkybox(const Ref<Texture>& hdrTexture) override;

        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        Ref<Texture> CreateTexture(const std::string& filePath) override;
        Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;
        Ref<Texture> CreateHDRTexture(const std::string& filePath) override;
        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

        struct FrameResources {
            Scope<VulkanCommandBuffer> cmdBuffer;
            VkSemaphore imageAvaliableSemaphore;
            VkFence inFlightFence;
        };

        static constexpr uint32_t MAX_FLIGHT_FRAMES = 3;

    private:
        VulkanContextManager m_CtxManager;
        uint32_t m_ImageIndex = 0;
        uint32_t m_CurrentFrameIndex = 0;
        std::array<FrameResources, MAX_FLIGHT_FRAMES> m_Frames;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        VkViewport m_Viewport;

        Ref<VulkanGraphicPipeline> m_Pipeline;

        Ref<VulkanUniformBuffer> m_Ubo;
        Ref<VulkanVertexBuffer> m_ColorQuadVbo;
        Ref<VulkanIndexBuffer> m_ColorQuadIbo;

        BufferData m_BufferData{};

        // 动态渲染函数指针
        PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = nullptr;
        PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = nullptr;

        void DestroyFrameResources();
    };
}