#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include "glm/glm.hpp"

namespace Azer {

    struct VulkanContext;
    class VulkanGraphicPipeline;

    struct BufferData
    {
        glm::mat4 viewProjMat = glm::mat4(1.0);
        glm::mat4 modelMat = glm::mat4(1.0);
    };

    class VulkanUniformBuffer 
    {
    public:
        VulkanUniformBuffer(const Ref<VulkanContext>& ctx);
        ~VulkanUniformBuffer();

        void Upload(const BufferData& data, const VkCommandBuffer& cmd, const Ref<VulkanGraphicPipeline>& pipeline);

    private:
        Ref<VulkanContext> m_Context;

        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;

        uint32_t findMemoryType(VkPhysicalDevice physicalDevice, 
                            uint32_t resourceMemoryTypeBits, 
                            VkMemoryPropertyFlags requiredProperties);
    };
}