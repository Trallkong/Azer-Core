#include "azpch.h"
#include "VulkanGraphicPipeline.h"
#include "VulkanVertexAttributes.h"
#include "VulkanUniformBuffer.h"
#include "FileSystem.h"
#include "Mesh2D.h"

namespace Azer {
    VulkanGraphicPipeline::VulkanGraphicPipeline()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

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
        rasterizerInfo.frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizerInfo.depthBiasEnable          = VK_FALSE;
        rasterizerInfo.lineWidth                = 1.0f;

        VkPipelineMultisampleStateCreateInfo multiSampleInfo {};
        multiSampleInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
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

        CreatePipelineLayout();

        m_VertexShader = CreateScope<VulkanShader>(FileSystem::ResolvePath("./assets/shaders/vulkan_vert.spv"), ShaderType::VERTEX);
        m_FragmentShader = CreateScope<VulkanShader>(FileSystem::ResolvePath("./assets/shaders/vulkan_frag.spv"), ShaderType::FRAGMENT);
        
        VkPipelineShaderStageCreateInfo stages[] = { m_VertexShader->GetStageInfo(), m_FragmentShader->GetStageInfo() };

        VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
        pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &ctx.SwapchainImageFormat;

        std::vector<VulkanVertexAtrribute> attributes = {
            { VulkanVertexAttributeType::Float3, "a_Position" },
            { VulkanVertexAttributeType::Float2, "a_TexCoord" },
        };

        VulkanVertexAttributes atbs(attributes);

        VkVertexInputBindingDescription bindingDesc = atbs.GetBindingDesc();
        bindingDesc.stride = sizeof(VertexData);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = atbs.GetDescriptions().data();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;

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
        pipelineInfo.pStages                = stages;
        pipelineInfo.pNext                  = &pipelineRenderingInfo;
        pipelineInfo.pVertexInputState      = &vertexInputInfo;

        VkPipelineCacheCreateInfo cacheInfo {};
        cacheInfo.sType     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        vkCreatePipelineCache(ctx.Device, &cacheInfo, nullptr, &m_Cache);
        vkCreateGraphicsPipelines(ctx.Device, m_Cache, 1, &pipelineInfo, nullptr, &m_GraphicPipeline);
    }

    VulkanGraphicPipeline::~VulkanGraphicPipeline()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        if (m_PipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(ctx.Device, m_PipelineLayout, nullptr);
        }

        if (m_GraphicPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(ctx.Device, m_GraphicPipeline, nullptr);
        }

        if (m_Cache != VK_NULL_HANDLE)
        {
            vkDestroyPipelineCache(ctx.Device, m_Cache, nullptr);
        }
    }

    void VulkanGraphicPipeline::CreatePipelineLayout()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        // Pipeline layout：set 0 = UBO，set 1 = texture
        VkDescriptorSetLayout pipelineSetLayouts[] = {
            ctx.UboSetLayout,
            ctx.TextureSetLayout
        };

        VkPushConstantRange pushRange{};
        pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushRange.offset = 0;
        pushRange.size = sizeof(DrawPushConstants);

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 2;
        layoutInfo.pSetLayouts = pipelineSetLayouts;
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushRange;

        vkCreatePipelineLayout(ctx.Device, &layoutInfo, nullptr, &m_PipelineLayout);
    }
}
