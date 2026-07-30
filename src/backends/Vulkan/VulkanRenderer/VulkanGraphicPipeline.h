#pragma once

#include "Base.h"

#include "vulkan/vulkan.h"

#include "VulkanContextManager.h"
#include "VulkanShader.h"

namespace Azer {

    class VulkanGraphicPipeline {
    public:
        static constexpr uint32_t MAX_FLIGHT_FRAMES = 3;

        VulkanGraphicPipeline(const Ref<VulkanContext>& context);
        ~VulkanGraphicPipeline();

        inline const VkPipeline& Get() const { return m_GraphicPipeline; }
        inline const VkPipelineLayout& Layout() const { return m_PipelineLayout; }
        inline const VkDescriptorSetLayout& DescriptorSetLayout() const { return m_SetLayout; }
        inline const VkDescriptorSet& DescriptorSet(uint32_t frameIndex) const { return m_Sets[frameIndex]; }
        inline VkDescriptorSet* DescriptorSetPtr(uint32_t frameIndex) { return &m_Sets[frameIndex]; }

    private:
        Ref<VulkanContext> m_Context;
        Ref<VulkanShader> m_VertexShader;
        Ref<VulkanShader> m_FragmentShader;

        VkPipelineCache m_Cache;
        VkPipeline m_GraphicPipeline;
        VkPipelineLayout m_PipelineLayout;

        VkDescriptorSetLayout m_SetLayout;
        std::array<VkDescriptorSet, MAX_FLIGHT_FRAMES> m_Sets{};

        void CreatePipelineLayout();
    };
}
