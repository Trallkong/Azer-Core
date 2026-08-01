#include "azpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContextManager.h"

namespace Azer {

    VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
        : m_Size(size)
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_Size;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                        | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        VmaAllocationInfo allocResult{};
        vmaCreateBuffer(ctx.Allocator, &bufferInfo, &allocInfo,
            &m_Buffer, &m_Allocation, &allocResult);
        m_MappedData = allocResult.pMappedData;
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(ctx.Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanUniformBuffer::SetData(const void* data, uint32_t size)
    {
        memcpy(m_MappedData, data, size);
    }

    void VulkanUniformBuffer::SetData(const void* data, uint32_t size, VkDeviceSize offset)
    {
        memcpy(static_cast<uint8_t*>(m_MappedData) + offset, data, size);
    }
}
