#include "azpch.h"
#include "VulkanStagingBuffer.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanStagingBuffer::VulkanStagingBuffer(const Ref<VulkanContext> &ctx, uint32_t size)
        : m_Context(ctx)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.size = size;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                        | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        VmaAllocationInfo allocResult{};
        vmaCreateBuffer(m_Context->Allocator, &bufferInfo, &allocInfo,
            &m_Buffer, &m_Allocation, &allocResult);
        m_MappedData = allocResult.pMappedData;
    }

    VulkanStagingBuffer::~VulkanStagingBuffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_Context->Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanStagingBuffer::Upload(void *data, uint32_t size)
    {
        memcpy(m_MappedData, data, size);
    }
}
