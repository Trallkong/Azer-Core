#include "azpch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanCommandBuffer::VulkanCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanContextManager::GetContext().cmdPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // 主命令缓冲
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(VulkanContextManager::GetContext().Device, &allocInfo, &m_Buffer);
        if (result != VK_SUCCESS) {
            AZ_CORE_ERROR("Create CommandBuffer Failed!");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(VulkanContextManager::GetContext().Device, VulkanContextManager::GetContext().cmdPool, 1, &m_Buffer);
        }
    }

    void VulkanCommandBuffer::SubmitSingleTime(const std::function<void(const VkCommandBuffer&)>& record)
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        VulkanCommandBuffer cmdBuffer;
        const VkCommandBuffer& cmd = cmdBuffer.Get();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &beginInfo);

        record(cmd);

        vkEndCommandBuffer(cmd);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VkFence fence = VK_NULL_HANDLE;
        vkCreateFence(ctx.Device, &fenceInfo, nullptr, &fence);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        vkQueueSubmit(ctx.GraphicsQueue, 1, &submitInfo, fence);
        vkWaitForFences(ctx.Device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(ctx.Device, fence, nullptr);
    }
}