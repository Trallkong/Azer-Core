#include "azpch.h"
#include "VertexBuffer.h"

#include "RendererAPI.h"

#include "VulkanVertexBuffer.h"

namespace Azer
{
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanVertexBuffer>(size);
        default:
            AZ_CORE_WARN("VertexBuffer::Create: current backend does not support vertex buffers");
            return nullptr;
        }
    }
}
