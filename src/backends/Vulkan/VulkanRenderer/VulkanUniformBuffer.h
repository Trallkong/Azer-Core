#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include "glm/glm.hpp"

#include "vk_mem_alloc.h"

namespace Azer {

    struct VulkanContext;
    class VulkanGraphicPipeline;

    struct BufferData
    {
        glm::mat4 viewProjMat = glm::mat4(1.0);
        glm::mat4 modelMat = glm::mat4(1.0);
        glm::vec4 color = glm::vec4(1.0f);
    };

    class VulkanUniformBuffer 
    {
    public:
        VulkanUniformBuffer(const Ref<VulkanContext>& ctx);
        ~VulkanUniformBuffer();

        void Upload(const BufferData& data);
        void Bind(const VkCommandBuffer& cmd, const Ref<VulkanGraphicPipeline>& pipeline, uint32_t frameIndex);

        void InitDescriptor(const Ref<VulkanGraphicPipeline>& pipeline, uint32_t frameIndex);

    private:
        Ref<VulkanContext> m_Context;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
    };
}
