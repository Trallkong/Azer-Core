#pragma once

#include "vulkan/vulkan.h"

namespace Azer {

    class VulkanContext;

    class VulkanCommandBuffer {
    public:
        VulkanCommandBuffer(const VulkanContext& ctx);
        ~VulkanCommandBuffer();

        inline const VkCommandBuffer& Get() const { return m_Buffer; }
        inline VkCommandBuffer& Get() { return m_Buffer; }
    private:
        VkCommandBuffer m_Buffer;
    };
}