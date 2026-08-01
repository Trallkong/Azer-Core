#include "azpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "VulkanShader.h"

namespace Azer
{
    Ref<Shader> Shader::Create(const std::string& name)
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::Vulkan:
            return VulkanShader::Create(name);
        default:
            AZ_CORE_WARN("Shader::Create: current backend does not support custom shaders ('{0}')", name);
            return nullptr;
        }
    }
}
