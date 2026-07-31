#include "azpch.h"
#include "VulkanRenderer.h"

#include "vulkan/vulkan.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"


#include "Mesh2D.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanTexture.h"

namespace Azer {

    VulkanRenderer::~VulkanRenderer()
    {
    }

    bool VulkanRenderer::Initialize(Window *window)
    {
        m_CtxManager.Init(window);

        const Ref<VulkanContext>& ctx = m_CtxManager.GetContext();

        m_Pipeline = CreateRef<VulkanGraphicPipeline>(ctx);
        m_Ubo = CreateRef<VulkanUniformBuffer>(ctx);

        for (uint32_t i = 0; i < MAX_FLIGHT_FRAMES; i++)
        {
            m_Ubo->InitDescriptor(m_Pipeline, i);
        }

        m_ColorQuadVbo = CreateRef<VulkanVertexBuffer>(ctx, 4 * sizeof(VertexData));
        m_ColorQuadIbo = CreateRef<VulkanIndexBuffer>(ctx, 6 * 4);

        {
            QuadMesh unit;
            unit.SetSize({1.0f, 1.0f});
            m_ColorQuadVbo->Upload(unit.GetVertices());
            m_ColorQuadIbo->Upload(unit.GetIndices());
        }

        // 空白纹理：绘制纯色块时绑定到 set 1，保证 shader 采样为白色
        {
            uint32_t whitePixel = 0xFFFFFFFF;
            m_WhiteTexture = CreateRef<VulkanTexture>(1, 1, &whitePixel);
        }

        // 初始化 Viewport
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = window->GetWindowSize().width;
        viewport.height = window->GetWindowSize().height;
        viewport.minDepth = 0;
        viewport.maxDepth = 1;
        m_Viewport = viewport;
        
        // 初始化帧资源
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto& frame : m_Frames)
        {
            frame.cmdBuffer = CreateScope<VulkanCommandBuffer>(m_CtxManager.GetContext());

            VkResult result = vkCreateFence(ctx->Device, &fenceInfo, nullptr, &frame.inFlightFence);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create fence");

            result = vkCreateSemaphore(ctx->Device, &semaphoreInfo, nullptr, &frame.imageAvaliableSemaphore);
            AZ_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed");
        }

        m_RenderFinishedSemaphores.resize(ctx->SwapchainImages.size());
        for (auto& sem : m_RenderFinishedSemaphores)
        {
            VkResult result = vkCreateSemaphore(ctx->Device, &semaphoreInfo, nullptr, &sem);
            AZ_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed");
        }
    
        // 从设备加载函数指针
        m_vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)
            vkGetDeviceProcAddr(ctx->Device, "vkCmdBeginRenderingKHR");
        m_vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)
            vkGetDeviceProcAddr(ctx->Device, "vkCmdEndRenderingKHR");
        
        // 检查是否加载成功
        if (m_vkCmdBeginRenderingKHR == nullptr || m_vkCmdEndRenderingKHR == nullptr) {
            // 处理错误：动态渲染不可用
            AZ_ERROR("Dynamic rendering functions not available!");
        }

        return true;
    }

    void VulkanRenderer::Shutdown()
    {
        Ref<VulkanContext> ctx = m_CtxManager.GetContext();
        vkDeviceWaitIdle(ctx->Device);

        m_Pipeline.reset();
        m_Ubo.reset();
        m_ColorQuadIbo.reset();
        m_ColorQuadVbo.reset();
        m_WhiteTexture.reset();

        DestroyFrameResources();
        ImGuiShutdown();

        m_CtxManager.Shutdown();
    }

    void VulkanRenderer::BeginFrame(const glm::vec3 &clearColor)
    {
        Ref<VulkanContext> ctx = m_CtxManager.GetContext();

        vkWaitForFences(ctx->Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(ctx->Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence);

        vkAcquireNextImageKHR(ctx->Device, ctx->Swapchain, UINT64_MAX,
            m_Frames[m_CurrentFrameIndex].imageAvaliableSemaphore, nullptr, &m_ImageIndex);

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
        barrier.image = ctx->SwapchainImages[m_ImageIndex];
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
        renderingInfo.renderArea = { 0, 0, ctx->SwapchainImageExtent.width, ctx->SwapchainImageExtent.height};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        VkRenderingAttachmentInfoKHR colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachment.imageView = ctx->SwapchainImageViews[m_ImageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = { clearColor.r, clearColor.g, clearColor.b, 1.0f };
        renderingInfo.pColorAttachments = &colorAttachment;
        
        if (m_vkCmdBeginRenderingKHR) {
            m_vkCmdBeginRenderingKHR(cmd, &renderingInfo);
        }

        vkCmdBindPipeline(cmd, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->Get());

        vkCmdSetViewport(cmd, 0, 1, &m_Viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = ctx->SwapchainImageExtent;
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }
    
    void VulkanRenderer::EndFrame()
    {
        SetImGuiDrawData(ImGui::GetDrawData());

        Ref<VulkanContext> ctx = m_CtxManager.GetContext();

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
        presentBarrier.image = ctx->SwapchainImages[m_ImageIndex];
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

        VkSemaphore waitSemaphores[] = { m_Frames[m_CurrentFrameIndex].imageAvaliableSemaphore };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = waitSemaphores;

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_ImageIndex] };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(ctx->GraphicsQueue, 1, &submit_info, m_Frames[m_CurrentFrameIndex].inFlightFence);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &ctx->Swapchain;
        present_info.pImageIndices = &m_ImageIndex;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signalSemaphores;
        
        vkQueuePresentKHR(ctx->GraphicsQueue, &present_info);

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FLIGHT_FRAMES;
    }

    void VulkanRenderer::SetCamera(Camera &camera)
    {
        m_BufferData.viewProjMat = camera.GetViewProjectionMatrix();
        m_BufferData.modelMat = glm::mat4(1.0);
        uint32_t frame = m_CurrentFrameIndex;
        m_Ubo->Upload(m_BufferData);
        m_Ubo->Bind(m_Frames[frame].cmdBuffer->Get(), m_Pipeline, frame);
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
        m_Viewport = { (float)offsetX, (float)offsetY, (float)width, (float)height };
    }

    void VulkanRenderer::Resize(uint32_t width, uint32_t height)
    {
        vkDeviceWaitIdle(m_CtxManager.GetContext()->Device);
        m_CtxManager.ReCreateSwapchain(width, height);
    }

    void VulkanRenderer::DrawQuad(const Transform2D& transform, float alpha)
    {
        DrawColorQuad(transform, {1.0f, 1.0f, 1.0f, 1.0f});
    }

    void VulkanRenderer::DrawColorQuad(const Transform2D& transform, const glm::vec4 &color)
    {
        const VkCommandBuffer& cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();

        m_BufferData.modelMat = transform.GetMatrix();
        m_BufferData.color = color;

        uint32_t frame = m_CurrentFrameIndex;
        m_Ubo->Upload(m_BufferData);
        m_Ubo->Bind(m_Frames[frame].cmdBuffer->Get(), m_Pipeline, frame);

        // 纯色块绑定空白纹理到 set 1
        m_WhiteTexture->Bind(cmd, m_Pipeline->Layout());

        m_ColorQuadVbo->Bind(cmd);
        m_ColorQuadIbo->Bind(cmd);

        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    }

    void VulkanRenderer::DrawTexture(Texture *tex, const SDL_FRect &src, const Transform2D& transform, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawTexture not implemented yet");
    }

    void VulkanRenderer::DrawCube(const Transform3D& transform)
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
        const Ref<VulkanContext>& ctx = m_CtxManager.GetContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForVulkan(window);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.ApiVersion = VK_API_VERSION_1_3;
        init_info.Instance = ctx->Instance;
        init_info.PhysicalDevice = ctx->PhysicalDevice;
        init_info.Device = ctx->Device;
        init_info.QueueFamily = ctx->QueueFamilyIndex;
        init_info.Queue = ctx->GraphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = ctx->ImGuiDescriptorPool;
        init_info.MinImageCount = 2;
        init_info.ImageCount = MAX_FLIGHT_FRAMES;
        init_info.Allocator = nullptr;

        init_info.UseDynamicRendering = true;

        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.CheckVkResultFn = check_vk_result;

        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ctx->SwapchainImageFormat;

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

    Ref<Framebuffer> VulkanRenderer::CreateFramebuffer(const FramebufferSpec &spec)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateFramebuffer not implemented yet");
        return Ref<Framebuffer>();
    }

    void VulkanRenderer::DestroyFrameResources()
    {
        const Ref<VulkanContext>& ctx = m_CtxManager.GetContext();

        for (auto& frame: m_Frames)
        {
            frame.cmdBuffer.reset();

            if (frame.inFlightFence != VK_NULL_HANDLE)
            {
                vkDestroyFence(ctx->Device, frame.inFlightFence, nullptr);
                AZ_CORE_DEBUG("Destroy Fence");
                frame.inFlightFence = VK_NULL_HANDLE;
            }

            if (frame.imageAvaliableSemaphore != VK_NULL_HANDLE) 
            {
                vkDestroySemaphore(ctx->Device, frame.imageAvaliableSemaphore, nullptr);
                AZ_CORE_DEBUG("Destroy Semaphore");
                frame.imageAvaliableSemaphore = VK_NULL_HANDLE;
            }
        }

        for (auto& sem : m_RenderFinishedSemaphores)
        {
            if (sem != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(ctx->Device, sem, nullptr);
                AZ_CORE_DEBUG("Destroy Semaphore");
                sem = VK_NULL_HANDLE;
            }
        }
        m_RenderFinishedSemaphores.clear();
    }
}