#include "azpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContextManager.h"

namespace Azer {

    VulkanUniformBuffer::VulkanUniformBuffer()
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(BufferData);
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

        // 创建 descriptor set（set 0：UBO），从静态上下文的 layout/pool 分配
        VkDescriptorSetAllocateInfo setAllocInfo{};
        setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setAllocInfo.descriptorPool = ctx.MyDescriptorPool;
        setAllocInfo.descriptorSetCount = 1;
        setAllocInfo.pSetLayouts = &ctx.UboSetLayout;
        vkAllocateDescriptorSets(ctx.Device, &setAllocInfo, &m_DescriptorSet);

        // 一次性把 buffer 写入 set
        VkDescriptorBufferInfo descBufferInfo{};
        descBufferInfo.buffer = m_Buffer;
        descBufferInfo.offset = 0;
        descBufferInfo.range = sizeof(BufferData);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSet;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &descBufferInfo;
        vkUpdateDescriptorSets(ctx.Device, 1, &write, 0, nullptr);
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        if (m_DescriptorSet != VK_NULL_HANDLE)
        {
            vkFreeDescriptorSets(ctx.Device, ctx.MyDescriptorPool, 1, &m_DescriptorSet);
        }

        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(ctx.Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanUniformBuffer::Upload(const BufferData& data)
    {
        memcpy(m_MappedData, &data, sizeof(BufferData));
    }

    void VulkanUniformBuffer::Bind(const VkCommandBuffer& cmd, VkPipelineLayout pipelineLayout) const
    {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
    }
}
