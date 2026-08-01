#pragma once

#include "Base.h"
#include "Texture.h"

#include <string>

#include "vulkan/vulkan.h"

#include "vk_mem_alloc.h"

namespace Azer {

    class VulkanTexture : public Texture
    {
    public:
        explicit VulkanTexture(const std::string& filePath, bool isHDR = false);
        VulkanTexture(uint32_t width, uint32_t height, void* data);
        ~VulkanTexture() override;

        inline uint32_t GetWidth() const override { return m_Width; };
        inline uint32_t GetHeight() const override { return m_Height; };

        inline const std::string& GetFilePath() const { return m_FilePath; } 

        void* GetHandle() const override;

        // 用 shader 的布局把本纹理绑定到 set binding，直接录制到当前帧命令缓冲
        void Bind(uint32_t binding, const Ref<Shader>& shader) override;

        inline const VkDescriptorSet& GetDescriptorSet() const { return m_DescriptorSet; }
        inline const VkImageView& GetImageView() const { return m_ImageView; }
        inline const VkSampler& GetSampler() const { return m_Sampler; }

    private:
        void CreateFromData(void* data, uint32_t width, uint32_t height, VkFormat format, uint32_t bytesPerPixel);

        std::string m_FilePath;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

        VkImage m_Image = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VkSampler m_Sampler = VK_NULL_HANDLE;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    };
}
