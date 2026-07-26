#pragma once

#include "Base.h"

#include <string>

#include "vulkan/vulkan.h"

namespace Azer {

    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    class VulkanShader {
    public:
        VulkanShader(VkDevice device, const std::string& filePath, ShaderType type, const std::string& entryPoint = "main");
        ~VulkanShader();

    private:
        VkDevice m_Device;
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