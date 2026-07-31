#include "azpch.h"
#include "VulkanIndexBuffer.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanIndexBuffer::VulkanIndexBuffer(uint32_t size)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                        | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        VmaAllocationInfo allocResult{};
        vmaCreateBuffer(VulkanContextManager::GetContext().Allocator, &bufferInfo, &allocInfo,
            &m_Buffer, &m_Allocation, &allocResult);
        m_MappedData = allocResult.pMappedData;
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(VulkanContextManager::GetContext().Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanIndexBuffer::Upload(const Indices& indices)
    {
        uint32_t size = indices.size() * sizeof(uint32_t);
        m_Count = indices.size();
        memcpy(m_MappedData, indices.data(), size);
    }

    void VulkanIndexBuffer::Bind(const VkCommandBuffer& cmd)
    {
        vkCmdBindIndexBuffer(cmd, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}
