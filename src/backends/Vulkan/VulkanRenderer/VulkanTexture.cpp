#include "azpch.h"
#include "VulkanTexture.h"

#include "stb_image.h"

#include "VulkanContextManager.h"
#include "VulkanImageTransition.h"
#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "VulkanStagingBuffer.h"
#include "VulkanCommandBuffer.h"

namespace Azer {

    VulkanTexture::VulkanTexture(const std::string& filePath, bool isHDR)
        : m_FilePath(filePath)
    {
        // 方向约定：V=0 对应原图顶部。Y 轴翻转统一由后端负高度视口处理，
        // 这里不能再翻转 stb 行序，否则会双翻转导致纹理上下颠倒。
        stbi_set_flip_vertically_on_load(false);

        int w, h, channels;
        if (isHDR)
        {
            float* data = stbi_loadf(filePath.c_str(), &w, &h, &channels, 4);
            if (!data)
            {
                AZ_CORE_ERROR("Failed to load HDR image: {0}", filePath);
                return;
            }
            CreateFromData(data, static_cast<uint32_t>(w), static_cast<uint32_t>(h),
                VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4);
            stbi_image_free(data);
        }
        else
        {
            // 强制 4 通道：R8G8B8 等 3 通道格式在 OPTIMAL tiling 下多数 GPU 不支持
            unsigned char* data = stbi_load(filePath.c_str(), &w, &h, &channels, 4);
            if (!data)
            {
                AZ_CORE_ERROR("Failed to load image: {0}", filePath);
                return;
            }

            CreateFromData(data, static_cast<uint32_t>(w), static_cast<uint32_t>(h),
                VK_FORMAT_R8G8B8A8_SRGB, 4);
            stbi_image_free(data);
        }
    }

    VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, void *data)
    {
        CreateFromData(data, width, height, VK_FORMAT_R8G8B8A8_SRGB, 4);
    }

    VulkanTexture::~VulkanTexture()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        // 确保 GPU 不再使用该纹理的 descriptor set / image
        vkDeviceWaitIdle(ctx.Device);

        // descriptor set 由 VulkanDescriptorAllocator 的池统一管理，池销毁时隐式释放

        if (m_Sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(ctx.Device, m_Sampler, nullptr);
        }

        if (m_ImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(ctx.Device, m_ImageView, nullptr);
        }

        if (m_Image != VK_NULL_HANDLE)
        {
            vmaDestroyImage(ctx.Allocator, m_Image, m_Allocation);
        }
    }

    void* VulkanTexture::GetHandle() const
    {
        return reinterpret_cast<void*>(m_DescriptorSet);
    }

    void VulkanTexture::Bind(uint32_t binding, const Ref<Shader>& shader)
    {
        VulkanRenderer* renderer = VulkanRenderer::Get();
        auto* vkShader = dynamic_cast<VulkanShader*>(shader.get());
        if (renderer == nullptr || vkShader == nullptr)
        {
            return;
        }

        // 用 shader 的布局直接录制到当前帧命令缓冲（布局与绘制时一致，天然兼容）
        vkCmdBindDescriptorSets(renderer->GetCurrentFrameCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkShader->GetLayout(), binding, 1, &m_DescriptorSet, 0, nullptr);
    }

    void VulkanTexture::CreateFromData(void* data, uint32_t width, uint32_t height, VkFormat format, uint32_t bytesPerPixel)
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        m_Width = width;
        m_Height = height;

        // 1. 创建 GPU 图像
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = { width, height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        VkResult result = vmaCreateImage(ctx.Allocator, &imageInfo, &allocInfo,
            &m_Image, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            AZ_CORE_ERROR("Failed to create image with VMA.");
            return;
        }

        // 2. staging buffer 拷贝像素数据
        VkDeviceSize imageSize = static_cast<VkDeviceSize>(width) * height * bytesPerPixel;
        VulkanStagingBuffer staging(static_cast<uint32_t>(imageSize));
        staging.Upload(data, static_cast<uint32_t>(imageSize));

        // 3. 一次性命令：layout transition + copy + transition（同步等待 GPU 完成）
        VulkanCommandBuffer::SubmitSingleTime([&](const VkCommandBuffer& cmd) {
            VulkanImageTransition::ToTransferDst(cmd, m_Image);

            VkBufferImageCopy region{};
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = { width, height, 1 };
            vkCmdCopyBufferToImage(cmd, staging.GetBuffer(), m_Image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            VulkanImageTransition::ToShaderRead(cmd, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        });

        // 4. ImageView
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(ctx.Device, &viewInfo, nullptr, &m_ImageView);

        // 5. Sampler
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkCreateSampler(ctx.Device, &samplerInfo, nullptr, &m_Sampler);

        // 6. Descriptor set（set 1, binding 0 = combined image sampler）
        // 使用共享的标准纹理布局，与 shader 反射出的 set 1 布局签名一致
        VkDescriptorSetLayout textureLayout = VulkanShader::GetStandardTextureLayout();
        m_DescriptorSet = ctx.DescriptorAllocator.Allocate(textureLayout);

        VkDescriptorImageInfo imageInfoDesc{};
        imageInfoDesc.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfoDesc.imageView = m_ImageView;
        imageInfoDesc.sampler = m_Sampler;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSet;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfoDesc;
        vkUpdateDescriptorSets(ctx.Device, 1, &write, 0, nullptr);
    }
}
