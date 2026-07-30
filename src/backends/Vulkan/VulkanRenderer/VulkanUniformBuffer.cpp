#include "azpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContextManager.h"
#include "VulkanGraphicPipeline.h"

namespace Azer {

    VulkanUniformBuffer::VulkanUniformBuffer(const Ref<VulkanContext>& ctx)
        : m_Context(ctx)
    {
        {   // 分配内存
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = sizeof(BufferData);
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            vkCreateBuffer(m_Context->Device, &bufferInfo, nullptr, &m_Buffer);

            VkMemoryRequirements finalMemReqs;
            vkGetBufferMemoryRequirements(m_Context->Device, m_Buffer, &finalMemReqs);

            uint32_t typeIndex = findMemoryType(
                m_Context->PhysicalDevice,
                finalMemReqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memAllocInfo.memoryTypeIndex = typeIndex;
            memAllocInfo.allocationSize = sizeof(BufferData);

            vkAllocateMemory(m_Context->Device, &memAllocInfo, nullptr, &m_Memory);

            vkBindBufferMemory(m_Context->Device, m_Buffer, m_Memory, 0);
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        if (m_Memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_Context->Device, m_Memory, nullptr);
        }

        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_Context->Device, m_Buffer, nullptr);
        }
    }

    void VulkanUniformBuffer::Upload(const BufferData& data, const VkCommandBuffer& cmd, const Ref<VulkanGraphicPipeline>& pipeline)
    {
        void* mappedData;
        VkResult result = vkMapMemory(m_Context->Device, m_Memory, 0, VK_WHOLE_SIZE, 0, &mappedData);
        if (result != VK_SUCCESS)
        {
            AZ_CORE_ERROR("获取物理内存地址失败");
            return;
        }

        memcpy(mappedData, &data, sizeof(BufferData));
        vkUnmapMemory(m_Context->Device, m_Memory);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(BufferData);

        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.descriptorCount = 1;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.dstBinding = 0;
        writeSet.dstSet = pipeline->DescriptorSet();
        writeSet.dstArrayElement = 0;
        writeSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Context->Device, 1, &writeSet, 0, nullptr);

        vkCmdBindDescriptorSets(cmd, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline->Layout(), 0, 1, &pipeline->DescriptorSet(), 0, nullptr
        );
    }

    // 功能：从设备属性中，找出符合 resourceMemoryTypeBits 要求，且同时具备所需 propertyFlags 的内存类型索引
    uint32_t VulkanUniformBuffer::findMemoryType(VkPhysicalDevice physicalDevice, 
                            uint32_t resourceMemoryTypeBits, 
                            VkMemoryPropertyFlags requiredProperties) {
        
        // 1. 获取设备的全部内存类型信息
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        // 2. 遍历所有内存类型 (通常是 0 到 31)
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            // 条件 A：当前类型 i 是否在资源允许的位掩码中？
            // 写法：(resourceMemoryTypeBits >> i) & 1
            // 或者更简洁的：resourceMemoryTypeBits & (1 << i)
            bool isSuitableForResource = (resourceMemoryTypeBits & (1 << i)) != 0;
            
            // 条件 B：当前类型 i 是否拥有我需要的所有 CPU/GPU 访问属性？
            // 注意：这里必须用 (required & properties) == required，表示“完全包含”
            bool hasRequiredProperties = (memProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties;

            if (isSuitableForResource && hasRequiredProperties) {
                return i; // 找到了！
            }
        }

        // 如果遍历完都没找到，说明硬件不支持或配置错误，需要抛出异常
        AZ_CORE_ERROR("没有找到合适的内存类型!");
    }
}