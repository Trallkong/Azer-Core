#include "azpch.h"
#include "VulkanGraphicPipeline.h"

namespace Azer {
    VulkanGraphicPipeline::VulkanGraphicPipeline(VulkanRendererContext* context)
        : m_Context(context)
    {
        VkPipelineInputAssemblyStateCreateInfo inputInfo {};
        inputInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputInfo.topology              = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStates.data();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

        VkPipelineViewportStateCreateInfo viewportInfo {};
        viewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizerInfo {};
        rasterizerInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable         = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable  = VK_FALSE;
        rasterizerInfo.polygonMode              = VK_POLYGON_MODE_FILL;
        rasterizerInfo.cullMode                 = VK_CULL_MODE_BACK_BIT;
        rasterizerInfo.frontFace                = VK_FRONT_FACE_CLOCKWISE;
        rasterizerInfo.depthBiasEnable          = VK_FALSE;
        rasterizerInfo.lineWidth                = 1.0f;

        VkPipelineMultisampleStateCreateInfo multiSampleInfo {};
        multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multiSampleInfo.sampleShadingEnable  = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorAttachment {};
        colorAttachment.blendEnable = VK_TRUE;
        colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
        colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
        colorAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo {};
        colorBlendStateInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments    = &colorAttachment;
        colorBlendStateInfo.logicOpEnable   = VK_FALSE;
        colorBlendStateInfo.logicOp         = VK_LOGIC_OP_COPY;

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pushConstantRangeCount = 0;
        vkCreatePipelineLayout(m_Context->GetContext().Device, &layoutInfo, nullptr, &m_PipelineLayout);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pDynamicState          = &dynamicState;
        pipelineInfo.pViewportState         = &viewportInfo;
        pipelineInfo.pInputAssemblyState    = &inputInfo;
        pipelineInfo.layout                 = m_PipelineLayout;
        pipelineInfo.pColorBlendState       = &colorBlendStateInfo;
        pipelineInfo.pDepthStencilState     = nullptr;
        pipelineInfo.renderPass             = nullptr;
        pipelineInfo.pMultisampleState      = &multiSampleInfo;
        pipelineInfo.pRasterizationState    = &rasterizerInfo;
        pipelineInfo.stageCount             = 2;

        VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &m_Context->GetContext().SwapchainImageFormat;

        VkPipelineCacheCreateInfo cacheInfo {};
        cacheInfo.sType     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        vkCreatePipelineCache(m_Context->GetContext().Device, &cacheInfo, nullptr, &m_Cache);
        vkCreateGraphicsPipelines(m_Context->GetContext().Device, m_Cache, 1, &pipelineInfo, nullptr, &m_GraphicPipeline);
    }

    VulkanGraphicPipeline::~VulkanGraphicPipeline()
    {
        
    }
}
