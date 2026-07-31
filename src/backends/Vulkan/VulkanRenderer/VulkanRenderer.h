#pragma once

#include <vector>

#include "Base.h"
#include "Renderer.h"

#include "VulkanContextManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicPipeline.h"
#include "VulkanUniformBuffer.h"
#include "VulkanMeshPool.h"

namespace Azer {

    class VulkanVertexBuffer;
    class VulkanIndexBuffer;
    class VulkanTexture;

    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer() = default;

        ~VulkanRenderer() override;

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
        void DrawTexture(const Ref<Texture>& tex, const Transform2D& transform, float alpha = 1.0f) override;

        void DrawCube(const Transform3D& transform) override;
        void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha = 1.0f) override;
        void DrawSkybox(const Ref<Texture>& hdrTexture) override;

        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

        struct FrameResources {
            Scope<VulkanCommandBuffer> cmdBuffer;
            VkSemaphore imageAvaliableSemaphore;
            VkFence inFlightFence;

            Ref<VulkanUniformBuffer> ubo;
        };

        static constexpr uint32_t MAX_FLIGHT_FRAMES = 3;

    private:
        VulkanContextManager m_CtxManager;
        Window* m_Window = nullptr;

        uint32_t m_ImageIndex = 0;
        uint32_t m_CurrentFrameIndex = 0;
        std::array<FrameResources, MAX_FLIGHT_FRAMES> m_Frames;
        
        // 提交完成信号量：按交换链图像索引（而非帧索引）分配。
        // 帧 fence 只能保证 submit 完成，不能保证 present 完成；
        // 只有 vkAcquireNextImageKHR 返回图像 N 时，才能确定图像 N 上一次的 present 已结束，
        // 此时该信号量才可安全复用（见 VUID-vkQueueSubmit-pSignalSemaphores-00067）。
        std::vector<VkSemaphore> m_SubmitSemaphores;
        
        VkViewport m_Viewport;

        Ref<VulkanGraphicPipeline> m_Pipeline;

        Ref<VulkanTexture> m_WhiteTexture;

        BufferData m_BufferData{};

        Scope<VulkanMeshPool> m_MeshPool;

        void RecreateSwapchainFromWindow();
        void RebuildSubmitSemaphores();
        void DestroyFrameResources();
    };
}