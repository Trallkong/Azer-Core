#pragma once

#include "Base.h"
#include "Framebuffer.h"

#include "vulkan/vulkan.h"

namespace Azer {

    class VulkanFrameBuffer : public Framebuffer {
    public:
        VulkanFrameBuffer(Renderer& renderer, const FramebufferSpec& spec);
        ~VulkanFrameBuffer();

        void Resize(uint32_t width, uint32_t height) override;
        Ref<Texture> GetColorTexture() const override;
        void* GetColorTextureHandle() const override;
        void* GetDepthTextureHandle() const override;
        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;


    private:
        Renderer& m_Renderer;
        FramebufferSpec m_Spec;

        VkFramebuffer m_Framebuffer;
        VkImage m_ColorImage;
        VkDeviceMemory m_ColorImageMemory;
        VkImageView m_ColorImageView;

        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;
    };
}