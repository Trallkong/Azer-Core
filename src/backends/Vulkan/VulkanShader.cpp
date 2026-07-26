#include "azpch.h"
#include "VulkanShader.h"

#include "FileSystem.h"

namespace Azer {

    VulkanShader::VulkanShader(VkDevice device, const std::string &filePath, ShaderType type, const std::string& entryPoint = "main")
        : m_Device(device), m_FilePath(filePath), m_EntryPoint(entryPoint), m_Type(type)    
    {
        std::vector<uint8_t> shaderBytes = FileSystem::ReadBytes(m_FilePath);
        CreateShaderModule(shaderBytes);
        CreateShaderStage();
    }

    VulkanShader::~VulkanShader()
    {
    }

    void VulkanShader::CreateShaderModule(const std::vector<uint8_t> &shaderBytes)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderBytes.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBytes.data());

        VkResult result = vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ShaderModule);
        if (result != VK_SUCCESS) {
            AZ_CORE_ERROR("Failed to create shader module for file: {0}", m_FilePath);
        }
    }

    void VulkanShader::CreateShaderStage()
    {
        switch (m_Type) {
        case ShaderType::VERTEX:
            CreateVertexShaderStageInfo();
            break;
        case ShaderType::FRAGMENT:
            CreateFragmentShaderStageInfo();
            break;
        default:
            AZ_CORE_ERROR("Unsupported shader type for file: {0}", m_FilePath);
            break;
        }
    }

    void VulkanShader::CreateVertexShaderStageInfo()
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageInfo.module = m_ShaderModule;
        shaderStageInfo.pName = m_EntryPoint.c_str();
        m_ShaderStageInfo = shaderStageInfo;
    }

    void VulkanShader::CreateFragmentShaderStageInfo()
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageInfo.module = m_ShaderModule;
        shaderStageInfo.pName = m_EntryPoint.c_str();
        m_ShaderStageInfo = shaderStageInfo;
    }
}