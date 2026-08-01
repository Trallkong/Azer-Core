#include "azpch.h"
#include "VulkanVertexBuffer.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
        : m_Size(size)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(VulkanContextManager::GetContext().Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanVertexBuffer::Upload(const Vertices &vertices)
    {
        uint32_t size = vertices.size() * sizeof(VertexData);
        memcpy(m_MappedData, vertices.data(), size);
    }

    void VulkanVertexBuffer::Bind(const VkCommandBuffer &cmd)
    {
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &m_Buffer, &offset);
    }
}
