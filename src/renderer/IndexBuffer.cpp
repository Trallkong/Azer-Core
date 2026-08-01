#include "azpch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"

#include "VulkanIndexBuffer.h"

namespace Azer
{
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanIndexBuffer>(size);
        default:
            AZ_CORE_WARN("IndexBuffer::Create: current backend does not support index buffers");
            return nullptr;
        }
    }
}
