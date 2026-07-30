#pragma once

#include "Base.h"

#include "vulkan/vulkan.h"

#include "VulkanContextManager.h"
#include "VulkanShader.h"

namespace Azer {

    class VulkanGraphicPipeline {
    public:
        VulkanGraphicPipeline(const Ref<VulkanContext>& context);
        ~VulkanGraphicPipeline();

        inline const VkPipeline& Get() const { return m_GraphicPipeline; }
        inline const VkPipelineLayout& Layout() const { return m_PipelineLayout; }
        inline const VkDescriptorSet& DescriptorSet() const { return m_Set; }
        inline const VkDescriptorSetLayout& DescriptorSetLayout() const { return m_SetLayout; }

    private:
        Ref<VulkanContext> m_Context;
        Ref<VulkanShader> m_VertexShader;
        Ref<VulkanShader> m_FragmentShader;

        VkPipelineCache m_Cache;
        VkPipeline m_GraphicPipeline;
        VkPipelineLayout m_PipelineLayout;

        VkDescriptorSetLayout m_SetLayout;
        VkDescriptorSet m_Set;

        void CreatePipelineLayout();
    };
}