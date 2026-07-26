#include "azpch.h"
#include "VulkanFrameBuffer.h"

namespace Azer {

    VulkanFrameBuffer::VulkanFrameBuffer(Renderer& renderer, const FramebufferSpec& spec)
        : m_Renderer(renderer), m_Spec(spec)
    {
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
    }

    void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::Resize not implemented yet");
    }

    Ref<Texture> VulkanFrameBuffer::GetColorTexture() const
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::GetColorTexture not implemented yet");
        return Ref<Texture>();
    }

    void* VulkanFrameBuffer::GetColorTextureHandle() const
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::GetColorTextureHandle not implemented yet");
        return nullptr;
    }

    void* VulkanFrameBuffer::GetDepthTextureHandle() const
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::GetDepthTextureHandle not implemented yet");
        return nullptr;
    }

    uint32_t VulkanFrameBuffer::GetWidth() const
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::GetWidth not implemented yet");
        return 0;
    }

    uint32_t VulkanFrameBuffer::GetHeight() const
    {
        AZ_ASSERT(false, "VulkanFrameBuffer::GetHeight not implemented yet");
        return 0;
    }
}