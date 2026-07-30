#pragma once 

#include "Base.h"

#include <vector>

#include "vulkan/vulkan.h"

#include "VulkanMesh.h"

namespace Azer {

    class VulkanContext;

    class VulkanVertexBuffer 
    {
    public:
        VulkanVertexBuffer(
            const Ref<VulkanContext>& ctx, 
            uint32_t size);
        ~VulkanVertexBuffer();

        void Upload(const Vertices& vertices);
    private:
        Ref<VulkanContext> m_Context;

        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;

        uint32_t findMemoryType(VkPhysicalDevice physicalDevice, 
                    uint32_t resourceMemoryTypeBits, 
                    VkMemoryPropertyFlags requiredProperties);
    };
}