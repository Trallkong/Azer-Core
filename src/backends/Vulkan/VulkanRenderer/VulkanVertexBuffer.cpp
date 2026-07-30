#include "azpch.h"
#include "VulkanVertexBuffer.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanVertexBuffer::VulkanVertexBuffer(
        const Ref<VulkanContext>& ctx, 
        uint32_t size)
        : m_Context(ctx)
    {
        VkBufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer(m_Context->Device, &info, nullptr, &m_Buffer);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(m_Context->Device, m_Buffer, &req);

        VkMemoryAllocateInfo memInfo{};
        memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memInfo.memoryTypeIndex = findMemoryType(
            m_Context->PhysicalDevice,
            req.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        );
        memInfo.allocationSize = size;
        vkAllocateMemory(m_Context->Device, &memInfo, nullptr, &m_Memory);

        vkBindBufferMemory(m_Context->Device, m_Buffer, m_Memory, 0);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
    }

    void VulkanVertexBuffer::Upload(const Vertices &vertices)
    {
        
    }

    uint32_t VulkanVertexBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t resourceMemoryTypeBits, VkMemoryPropertyFlags requiredProperties)
    {
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
