#include "azpch.h"
#include "VulkanRenderer.h"

#include "vulkan/vulkan.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

namespace Azer {

    VulkanRenderer::~VulkanRenderer()
    {
    }

    bool VulkanRenderer::Initialize(Window *window)
    {
        m_Context.Init(window);
        m_Frames.resize(MAX_FRAMES_IN_FLIGHT);

        const VulkanContext& ctx = m_Context.GetContext();
    
        // 从设备加载函数指针
        m_vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)
            vkGetDeviceProcAddr(ctx.Device, "vkCmdBeginRenderingKHR");
        m_vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)
            vkGetDeviceProcAddr(ctx.Device, "vkCmdEndRenderingKHR");
        
        // 检查是否加载成功
        if (m_vkCmdBeginRenderingKHR == nullptr || m_vkCmdEndRenderingKHR == nullptr) {
            // 处理错误：动态渲染不可用
            AZ_ERROR("Dynamic rendering functions not available!");
        }

        for (auto& f : m_Frames)
        {
            f.cmdBuffer = CreateScope<VulkanCommandBuffer>(m_Context.GetContext());

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            // ⭐ 关键：初始状态设为已触发，这样第一帧就能立即开始
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
                VkResult result = vkCreateFence(ctx.Device, &fenceInfo, nullptr, &f.inFlightFence);
                AZ_ASSERT(result == VK_SUCCESS, "Failed to create fence for frame %u", i);
            }

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkResult result = vkCreateSemaphore(ctx.Device, &semaphoreInfo, nullptr, &f.imageAvaliableSemphore);
            AZ_ASSERT(result == VK_SUCCESS);
        
            result = vkCreateSemaphore(ctx.Device, &semaphoreInfo, nullptr, &f.renderFinishedSemphore);
            AZ_ASSERT(result == VK_SUCCESS);
        }

        return true;
    }

    void VulkanRenderer::Shutdown()
    {
        // 1.等待GPU完成所有工作
        const VulkanContext& ctx = m_Context.GetContext();
        vkDeviceWaitIdle(ctx.Device);

        // 2.销毁帧资源
        DestroyFrameResources();

        // 3.销毁 ImGui 资源
        ImGuiShutdown();

        // 4. 销毁 Context 中的所有资源（由 VulkanRendererContext 负责）
        m_Context.Shutdown();
    }

    void VulkanRenderer::BeginFrame(const glm::vec3 &clearColor)
    {
        VulkanContext& ctx = m_Context.GetContext();

        vkWaitForFences(ctx.Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(ctx.Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence);

        vkAcquireNextImageKHR(ctx.Device, ctx.Swapchain, UINT64_MAX,
            m_Frames[m_CurrentFrameIndex].imageAvaliableSemphore, nullptr, &m_ImageIndex);

        VkCommandBuffer cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();
        vkResetCommandBuffer(cmd, 0);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &begin_info);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.image = ctx.SwapchainImages[m_ImageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    
        VkRenderingInfoKHR renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.renderArea = { 0, 0, ctx.SwapchainImageExtent.width, ctx.SwapchainImageExtent.height};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        VkRenderingAttachmentInfoKHR colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachment.imageView = ctx.SwapchainImageViews[m_ImageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = { clearColor.r, clearColor.g, clearColor.b, 1.0f };
        renderingInfo.pColorAttachments = &colorAttachment;
        
        if (m_vkCmdBeginRenderingKHR) {
            m_vkCmdBeginRenderingKHR(cmd, &renderingInfo);
        }

        SetImGuiDrawData(ImGui::GetDrawData());
    }
    
    void VulkanRenderer::EndFrame()
    {
        VulkanContext& ctx = m_Context.GetContext();

        VkCommandBuffer cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();
        
        if (m_vkCmdBeginRenderingKHR) {
            m_vkCmdEndRenderingKHR(cmd);
        }

        VkImageMemoryBarrier presentBarrier{};
        presentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        presentBarrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        presentBarrier.dstAccessMask = 0;
        presentBarrier.image = ctx.SwapchainImages[m_ImageIndex];
        presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentBarrier.subresourceRange.baseMipLevel = 0;
        presentBarrier.subresourceRange.levelCount = 1;
        presentBarrier.subresourceRange.baseArrayLayer = 0;
        presentBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &presentBarrier
        );
        
        vkEndCommandBuffer(cmd);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.pWaitDstStageMask = waitStages;

        VkSemaphore waitSemaphores[] = {m_Frames[m_CurrentFrameIndex].imageAvaliableSemphore};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = waitSemaphores;

        VkSemaphore signalSemaphores[] = {m_Frames[m_CurrentFrameIndex].renderFinishedSemphore};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(ctx.GraphicsQueue, 1, &submit_info, m_Frames[m_CurrentFrameIndex].inFlightFence);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &ctx.Swapchain;
        present_info.pImageIndices = &m_ImageIndex;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signalSemaphores;
        
        vkQueuePresentKHR(ctx.GraphicsQueue, &present_info);

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::SetCamera(Camera &camera)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetCamera not implemented yet");
    }

    void VulkanRenderer::ResetRenderState()
    {
        AZ_ASSERT(false, "VulkanRenderer::ResetRenderState not implemented yet");
    }

    void VulkanRenderer::SetRenderTarget(Framebuffer *target)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetRenderTarget not implemented yet");
    }

    void VulkanRenderer::SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetViewport not implemented yet");
    }

    void VulkanRenderer::DrawQuad(float x, float y, float w, float h, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawQuad not implemented yet");
    }

    void VulkanRenderer::DrawColorQuad(float x, float y, float w, float h, const glm::vec4 &color, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawColorQuad not implemented yet");
    }

    void VulkanRenderer::DrawTexture(Texture *tex, const SDL_FRect &src, const SDL_FRect &dst, float angle, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawTexture not implemented yet");
    }

    void VulkanRenderer::DrawCube(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawCube not implemented yet");
    }

    void VulkanRenderer::DrawModel(Model &model, const glm::mat4 &worldTransform, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawModel not implemented yet");
    }

    void VulkanRenderer::DrawSkybox(const Ref<Texture> &hdrTexture)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawSkybox not implemented yet");
    }

    static void check_vk_result(VkResult err)
    {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }

    void VulkanRenderer::ImGuiInit(SDL_Window *window)
    {
        const VulkanContext& ctx = m_Context.GetContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForVulkan(window);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.ApiVersion = VK_API_VERSION_1_3;
        init_info.Instance = ctx.Instance;
        init_info.PhysicalDevice = ctx.PhysicalDevice;
        init_info.Device = ctx.Device;
        init_info.QueueFamily = ctx.QueueFamilyIndex;
        init_info.Queue = ctx.GraphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = ctx.ImGuiDescriptorPool;
        init_info.MinImageCount = 2;
        init_info.ImageCount = static_cast<uint32_t>(ctx.SwapchainImages.size());
        init_info.Allocator = nullptr;

        init_info.UseDynamicRendering = true;

        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.CheckVkResultFn = check_vk_result;

        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ctx.SwapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);
    }

    void VulkanRenderer::ImGuiShutdown()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void VulkanRenderer::ImGuiNewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
    }

    void VulkanRenderer::SetImGuiDrawData(ImDrawData *drawData)
    {
        ImGui_ImplVulkan_RenderDrawData(drawData, m_Frames[m_CurrentFrameIndex].cmdBuffer->Get());
    }

    Ref<Texture> VulkanRenderer::CreateTexture(const std::string &filePath)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Texture> VulkanRenderer::CreateTexture(void *pixels, uint32_t width, uint32_t height)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Texture> VulkanRenderer::CreateHDRTexture(const std::string &filePath)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateHDRTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Framebuffer> VulkanRenderer::CreateFramebuffer(const FramebufferSpec &spec)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateFramebuffer not implemented yet");
        return Ref<Framebuffer>();
    }

    void VulkanRenderer::DestroyFrameResources()
    {
        const VulkanContext& ctx = m_Context.GetContext();

        for (auto& frame: m_Frames)
        {
            frame.cmdBuffer.reset();

            // 销毁Fence
            if (frame.inFlightFence != VK_NULL_HANDLE)
            {
                vkDestroyFence(ctx.Device, frame.inFlightFence, nullptr);
                frame.inFlightFence = VK_NULL_HANDLE;
            }

            // 销毁 Semaphore
            if (frame.imageAvaliableSemphore != VK_NULL_HANDLE) 
            {
                vkDestroySemaphore(ctx.Device, frame.imageAvaliableSemphore, nullptr);
                frame.imageAvaliableSemphore = VK_NULL_HANDLE;
            }

            if (frame.renderFinishedSemphore != VK_NULL_HANDLE) 
            {
                vkDestroySemaphore(ctx.Device, frame.renderFinishedSemphore, nullptr);
                frame.renderFinishedSemphore = VK_NULL_HANDLE;
            }
        }

        // 清空帧数组
        m_Frames.clear();
    }
}