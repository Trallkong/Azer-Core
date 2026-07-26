#include "azpch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRendererContext.h"

namespace Azer {

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanContext& ctx)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = ctx.cmdPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // 主命令缓冲
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(ctx.Device, &allocInfo, &m_Buffer);
        if (result != VK_SUCCESS) {
            AZ_CORE_ERROR("创建命令缓冲区失败");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        
    }
}