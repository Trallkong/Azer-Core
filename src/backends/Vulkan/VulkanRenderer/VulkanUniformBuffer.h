#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include "glm/glm.hpp"

#include "vk_mem_alloc.h"

namespace Azer {

    struct VulkanContext;

    struct BufferData
    {
        glm::mat4 viewProjMat = glm::mat4(1.0);
    };

    // 每次绘制各自的变换/颜色，走 push constants
    struct DrawPushConstants
    {
        glm::mat4 modelMat = glm::mat4(1.0);
        glm::vec4 color = glm::vec4(1.0f);
    };

    // 每个 UBO 实例自持 buffer / allocation / descriptor set / mapped data
    class VulkanUniformBuffer
    {
    public:
        VulkanUniformBuffer(const Ref<VulkanContext>& ctx);
        ~VulkanUniformBuffer();

        void Upload(const BufferData& data);
        void Bind(const VkCommandBuffer& cmd, VkPipelineLayout pipelineLayout) const;

    private:
        Ref<VulkanContext> m_Context;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    };
}
