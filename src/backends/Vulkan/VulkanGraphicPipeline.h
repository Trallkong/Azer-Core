#pragma once

#include "Base.h"

#include "vulkan/vulkan.h"

#include "VulkanRendererContext.h"
#include "VulkanShader.h"

namespace Azer {

    class VulkanGraphicPipeline {
    public:
        VulkanGraphicPipeline(VulkanRendererContext* context, VulkanShader);
        ~VulkanGraphicPipeline();

    private:
        VulkanRendererContext* m_Context;
        VkPipelineCache m_Cache;
        VkPipeline m_GraphicPipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}