#pragma once

#include "Base.h"

#include <string>
#include <vector>

#include "vulkan/vulkan.h"

namespace Azer {

    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    class VulkanContext;

    class VulkanShader {
    public:
        VulkanShader(const Ref<VulkanContext>& ctx, const std::string& filePath, ShaderType type, const std::string& entryPoint = "main");
        ~VulkanShader();

        inline VkPipelineShaderStageCreateInfo GetStageInfo() { return m_ShaderStageInfo; }
        inline VkShaderModule GetModule() { return m_ShaderModule; }

    private:
        Ref<VulkanContext> m_Context;
        std::string m_FilePath;
        std::string m_EntryPoint;
        ShaderType m_Type;

        VkShaderModule m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo;

        void CreateShaderModule(const std::vector<uint8_t>& shaderBytes);

        void CreateShaderStage();
        void CreateVertexShaderStageInfo();
        void CreateFragmentShaderStageInfo();
    };
}