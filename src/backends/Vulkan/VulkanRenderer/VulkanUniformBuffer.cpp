#include "azpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContextManager.h"
#include "VulkanGraphicPipeline.h"

namespace Azer {

    VulkanUniformBuffer::VulkanUniformBuffer(const Ref<VulkanContext>& ctx)
        : m_Context(ctx)
    {
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
        vmaCreateBuffer(m_Context->Allocator, &bufferInfo, &allocInfo,
            &m_Buffer, &m_Allocation, &allocResult);
        m_MappedData = allocResult.pMappedData;
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_Context->Allocator, m_Buffer, m_Allocation);
        }
    }

    void VulkanUniformBuffer::Upload(const BufferData& data)
    {
        memcpy(m_MappedData, &data, sizeof(BufferData));
    }

    void VulkanUniformBuffer::Bind(const VkCommandBuffer& cmd, const Ref<VulkanGraphicPipeline>& pipeline, uint32_t frameIndex)
    {
        vkCmdBindDescriptorSets(cmd, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline->Layout(), 0, 1, &pipeline->DescriptorSet(frameIndex), 0, nullptr
        );
    }

    void VulkanUniformBuffer::InitDescriptor(const Ref<VulkanGraphicPipeline>& pipeline, uint32_t frameIndex)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(BufferData);

        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.descriptorCount = 1;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.dstBinding = 0;
        writeSet.dstSet = pipeline->DescriptorSet(frameIndex);
        writeSet.dstArrayElement = 0;
        writeSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Context->Device, 1, &writeSet, 0, nullptr);
    }
}
