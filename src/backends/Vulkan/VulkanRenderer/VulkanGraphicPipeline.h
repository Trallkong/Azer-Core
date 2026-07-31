#pragma once

#include "Base.h"

#include "vulkan/vulkan.h"

#include "VulkanContextManager.h"
#include "VulkanShader.h"

namespace Azer {

    class VulkanGraphicPipeline {
    public:
        VulkanGraphicPipeline();
        ~VulkanGraphicPipeline();

        inline const VkPipeline& Get() const { return m_GraphicPipeline; }
        inline const VkPipelineLayout& Layout() const { return m_PipelineLayout; }

    private:
        Ref<VulkanShader> m_VertexShader;
        Ref<VulkanShader> m_FragmentShader;

        VkPipelineCache m_Cache;
        VkPipeline m_GraphicPipeline;
        VkPipelineLayout m_PipelineLayout;

        void CreatePipelineLayout();
    };
}
