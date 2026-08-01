#pragma once

#include <vector>

#include "Base.h"
#include "Renderer.h"

#include "VulkanContextManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanShader.h"

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
        void ResetRenderState() override;
        void SetRenderTarget(Framebuffer* target) override;  // nullptr = swapchain
        void Resize(uint32_t width, uint32_t height) override;
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override;
        uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

        void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                         const Ref<IndexBuffer>& indexBuffer,
                         const Ref<Shader>& shader) override;
        void Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                  const Ref<Shader>& shader) override;

        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

        // ---- 静态访问（后端单例）----
        static VulkanRenderer* Get() { return s_Instance; }

        // 当前在途帧的命令缓冲（BeginFrame 到 EndFrame 之间有效）
        VkCommandBuffer GetCurrentFrameCmdBuffer() const { return m_Frames[m_CurrentFrameIndex].cmdBuffer->Get(); }

        struct FrameResources {
            Scope<VulkanCommandBuffer> cmdBuffer;
            VkSemaphore imageAvaliableSemaphore;
            VkFence inFlightFence;
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

        // 深度附件（base3d 的 depth_test 需要）
        VkImage m_DepthImage = VK_NULL_HANDLE;
        VmaAllocation m_DepthAllocation = VK_NULL_HANDLE;
        VkImageView m_DepthImageView = VK_NULL_HANDLE;

        void RecreateSwapchainFromWindow();
        void RebuildSubmitSemaphores();
        void DestroyFrameResources();

        void CreateDepthResources();
        void DestroyDepthResources();
        void BindDrawState(const VulkanShader* vkShader);
        void BindUniformSets(const VulkanShader* vkShader);

        static VulkanRenderer* s_Instance;
    };
}
