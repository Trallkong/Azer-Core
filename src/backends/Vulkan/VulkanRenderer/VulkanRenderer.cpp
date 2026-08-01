#include "azpch.h"
#include "VulkanRenderer.h"

#include "vulkan/vulkan.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"


#include "VulkanImageTransition.h"
#include "VulkanDescriptorSet.h"
#include "VulkanTexture.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Azer {

    VulkanRenderer* VulkanRenderer::s_Instance = nullptr;

    VulkanRenderer::~VulkanRenderer()
    {
    }

    bool VulkanRenderer::Initialize(Window *window)
    {
        m_Window = window;
        m_CtxManager.Init(window);

        const VulkanContext& ctx = VulkanContextManager::GetContext();

        s_Instance = this;

        CreateDepthResources();

        // 初始化 Viewport
        // Vulkan 帧缓冲原点在左上、NDC Y 向下；用负高度视口在光栅化阶段翻转 Y，
        // 使世界 +Y 渲染在屏幕上方（与 2D/3D 统一，且不影响背面剔除绕序）。
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = static_cast<float>(window->GetWindowSize().height);
        viewport.width = static_cast<float>(window->GetWindowSize().width);
        viewport.height = -static_cast<float>(window->GetWindowSize().height);
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
            frame.cmdBuffer = CreateScope<VulkanCommandBuffer>();

            VkResult result = vkCreateFence(ctx.Device, &fenceInfo, nullptr, &frame.inFlightFence);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create fence");

            result = vkCreateSemaphore(ctx.Device, &semaphoreInfo, nullptr, &frame.imageAvaliableSemaphore);
            AZ_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed");
        }

        // 按交换链图像数量分配提交完成信号量（不能按帧索引复用）
        RebuildSubmitSemaphores();

        return true;
    }

    void VulkanRenderer::Shutdown()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();
        vkDeviceWaitIdle(ctx.Device);

        DestroyDepthResources();

        DestroyFrameResources();
        ImGuiShutdown();

        s_Instance = nullptr;

        // 销毁共享描述符布局（此时所有 VulkanShader / 纹理均已释放）
        VulkanShader::ShutdownSharedLayouts();

        m_CtxManager.Shutdown();
    }

    void VulkanRenderer::BeginFrame(const glm::vec3 &clearColor)
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        vkWaitForFences(ctx.Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(ctx.Device, 1, &m_Frames[m_CurrentFrameIndex].inFlightFence);

        VkResult acquireResult = vkAcquireNextImageKHR(ctx.Device, ctx.Swapchain->GetSwapchain(), UINT64_MAX,
            m_Frames[m_CurrentFrameIndex].imageAvaliableSemaphore, nullptr, &m_ImageIndex);

        // 交换链失效（窗口大小变化等）时先重建再重试获取
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchainFromWindow();
            acquireResult = vkAcquireNextImageKHR(ctx.Device, ctx.Swapchain->GetSwapchain(), UINT64_MAX,
                m_Frames[m_CurrentFrameIndex].imageAvaliableSemaphore, nullptr, &m_ImageIndex);
        }

        AZ_ASSERT(acquireResult == VK_SUCCESS || acquireResult == VK_SUBOPTIMAL_KHR,
            "Failed to acquire swapchain image");

        VkCommandBuffer cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();
        vkResetCommandBuffer(cmd, 0);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &begin_info);

        // 交换链颜色图：丢弃旧内容 -> 颜色附件布局
        VulkanImageTransition::ToColorAttachment(cmd, ctx.Swapchain->GetImage()[m_ImageIndex]);

        // 深度图像在创建时一次性转到 DEPTH_STENCIL_ATTACHMENT_OPTIMAL 后保持不变，
        // 每帧用 LOAD_OP_CLEAR 清零，无需每帧 barrier；帧间安全由 in-flight fence 保证
    
        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { 0, 0, ctx.Swapchain->GetExtent().width, ctx.Swapchain->GetExtent().height};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = ctx.Swapchain->GetImageViews()[m_ImageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = { clearColor.r, clearColor.g, clearColor.b, 1.0f };
        renderingInfo.pColorAttachments = &colorAttachment;

        // base3d 的 depth_test 需要深度附件
        VkRenderingAttachmentInfo depthAttachment{};
        depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachment.imageView = m_DepthImageView;
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil.depth = 1.0f;
        depthAttachment.clearValue.depthStencil.stencil = 0;
        renderingInfo.pDepthAttachment = &depthAttachment;
        
        vkCmdBeginRendering(cmd, &renderingInfo);

        vkCmdSetViewport(cmd, 0, 1, &m_Viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = ctx.Swapchain->GetExtent();
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }
    
    void VulkanRenderer::EndFrame()
    {
        SetImGuiDrawData(ImGui::GetDrawData());

        const VulkanContext& ctx = VulkanContextManager::GetContext();

        VkCommandBuffer cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();
        
        vkCmdEndRendering(cmd);

        // 颜色图 -> present 布局（渲染写、present 读，真实内存依赖）
        VulkanImageTransition::ToPresent(cmd, ctx.Swapchain->GetImage()[m_ImageIndex]);
        
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

        VkSemaphore signalSemaphores[] = { m_SubmitSemaphores[m_ImageIndex] };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(ctx.GraphicsQueue, 1, &submit_info, m_Frames[m_CurrentFrameIndex].inFlightFence);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &ctx.Swapchain->GetSwapchain();
        present_info.pImageIndices = &m_ImageIndex;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signalSemaphores;
        
        VkResult presentResult = vkQueuePresentKHR(ctx.GraphicsQueue, &present_info);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapchainFromWindow();
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FLIGHT_FRAMES;
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
        // 负高度视口：翻转 Y（同 Initialize）
        m_Viewport = { (float)offsetX, (float)(offsetY + height), (float)width, -(float)height };
    }

    void VulkanRenderer::Resize(uint32_t width, uint32_t height)
    {
        VulkanContextManager::GetContext().Swapchain->RecreateSwapchain(width, height);
        RebuildSubmitSemaphores();
        CreateDepthResources();
    }

    void VulkanRenderer::RecreateSwapchainFromWindow()
    {
        WindowSize size = m_Window->GetWindowSize();
        VulkanContextManager::GetContext().Swapchain->RecreateSwapchain(size.width, size.height);
        RebuildSubmitSemaphores();
        CreateDepthResources();
    }

    void VulkanRenderer::RebuildSubmitSemaphores()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        // RecreateSwapchain 内部已 vkDeviceWaitIdle，所有 present 均已完成，可安全销毁重建
        for (VkSemaphore sem : m_SubmitSemaphores)
        {
            if (sem != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(ctx.Device, sem, nullptr);
            }
        }
        m_SubmitSemaphores.clear();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        m_SubmitSemaphores.resize(ctx.Swapchain->GetImage().size());
        for (auto& sem : m_SubmitSemaphores)
        {
            VkResult result = vkCreateSemaphore(ctx.Device, &semaphoreInfo, nullptr, &sem);
            AZ_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed");
        }
    }

    void VulkanRenderer::CreateDepthResources()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();
        VkExtent2D extent = ctx.Swapchain->GetExtent();

        DestroyDepthResources();

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_D32_SFLOAT;
        imageInfo.extent = { extent.width, extent.height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        VkResult result = vmaCreateImage(ctx.Allocator, &imageInfo, &allocInfo,
            &m_DepthImage, &m_DepthAllocation, nullptr);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create depth image");

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_DepthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D32_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        result = vkCreateImageView(ctx.Device, &viewInfo, nullptr, &m_DepthImageView);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create depth image view");

        // 一次性布局转换：UNDEFINED -> DEPTH_STENCIL_ATTACHMENT_OPTIMAL。
        // 此后深度图保持该布局，每帧靠 LOAD_OP_CLEAR 清零，不再需要每帧 barrier。
        VulkanCommandBuffer::SubmitSingleTime([&](const VkCommandBuffer& cmd) {
            VulkanImageTransition::ToDepthAttachment(cmd, m_DepthImage);
        });
    }

    void VulkanRenderer::DestroyDepthResources()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        if (m_DepthImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(ctx.Device, m_DepthImageView, nullptr);
            m_DepthImageView = VK_NULL_HANDLE;
        }
        if (m_DepthImage != VK_NULL_HANDLE)
        {
            vmaDestroyImage(ctx.Allocator, m_DepthImage, m_DepthAllocation);
            m_DepthImage = VK_NULL_HANDLE;
            m_DepthAllocation = VK_NULL_HANDLE;
        }
    }

    void VulkanRenderer::BindDrawState(const VulkanShader* vkShader)
    {
        const VkCommandBuffer& cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader->GetPipeline());

        // 动态 viewport / scissor（当前绘制目标）
        VkViewport viewport = m_Viewport;
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = ctx.Swapchain->GetExtent();
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }

    void VulkanRenderer::BindUniformSets(const VulkanShader* vkShader)
    {
        // 绑定 shader 的 uniform 描述符集（当前帧，带动态偏移）
        vkShader->BindFrameUniformSets(m_Frames[m_CurrentFrameIndex].cmdBuffer->Get(), m_CurrentFrameIndex);
    }

    void VulkanRenderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                                     const Ref<IndexBuffer>& indexBuffer,
                                     const Ref<Shader>& shader)
    {
        auto* vkShader = dynamic_cast<VulkanShader*>(shader.get());
        auto* vkVbo = dynamic_cast<VulkanVertexBuffer*>(vertexBuffer.get());
        auto* vkIbo = dynamic_cast<VulkanIndexBuffer*>(indexBuffer.get());
        AZ_ASSERT(vkShader != nullptr && vkVbo != nullptr && vkIbo != nullptr,
            "DrawIndexed: Vulkan backend requires Vulkan shader / vertex / index buffers");
        if (vkShader == nullptr || vkVbo == nullptr || vkIbo == nullptr)
        {
            return;
        }

        const VkCommandBuffer& cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();

        BindDrawState(vkShader);
        BindUniformSets(vkShader);

        vkVbo->Bind(cmd);
        vkIbo->Bind(cmd);
        vkCmdDrawIndexed(cmd, vkIbo->GetIndexCount(), 1, 0, 0, 0);
    }

    void VulkanRenderer::Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                              const Ref<Shader>& shader)
    {
        auto* vkShader = dynamic_cast<VulkanShader*>(shader.get());
        auto* vkVbo = dynamic_cast<VulkanVertexBuffer*>(vertexBuffer.get());
        AZ_ASSERT(vkShader != nullptr && vkVbo != nullptr,
            "Draw: Vulkan backend requires Vulkan shader / vertex buffer");
        if (vkShader == nullptr || vkVbo == nullptr)
        {
            return;
        }

        const VkCommandBuffer& cmd = m_Frames[m_CurrentFrameIndex].cmdBuffer->Get();

        BindDrawState(vkShader);
        BindUniformSets(vkShader);

        vkVbo->Bind(cmd);
        vkCmdDraw(cmd, vertexCount, 1, 0, 0);
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
        const VulkanContext& ctx = VulkanContextManager::GetContext();

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
        init_info.ImageCount = MAX_FLIGHT_FRAMES;
        init_info.Allocator = nullptr;

        init_info.UseDynamicRendering = true;

        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.CheckVkResultFn = check_vk_result;

        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ctx.Swapchain->GetFormat();
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

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
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        for (auto& frame: m_Frames)
        {
            frame.cmdBuffer.reset();

            if (frame.inFlightFence != VK_NULL_HANDLE)
            {
                vkDestroyFence(ctx.Device, frame.inFlightFence, nullptr);
                AZ_CORE_DEBUG("Destroy Fence");
                frame.inFlightFence = VK_NULL_HANDLE;
            }

            if (frame.imageAvaliableSemaphore != VK_NULL_HANDLE) 
            {
                vkDestroySemaphore(ctx.Device, frame.imageAvaliableSemaphore, nullptr);
                AZ_CORE_DEBUG("Destroy Semaphore");
                frame.imageAvaliableSemaphore = VK_NULL_HANDLE;
            }
        }

        for (VkSemaphore sem : m_SubmitSemaphores)
        {
            if (sem != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(ctx.Device, sem, nullptr);
                AZ_CORE_DEBUG("Destroy Semaphore");
            }
        }
        m_SubmitSemaphores.clear();
    }
}
