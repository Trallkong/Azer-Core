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
}