#include "azpch.h"
#include "VulkanTexture.h"

#include "stb_image.h"

#include "VulkanContextManager.h"
#include "VulkanStagingBuffer.h"
#include "VulkanCommandBuffer.h"

namespace Azer {

    static void ResolveChannelFormat(int channels, VkFormat& format, uint32_t& bytesPerPixel)
    {
        switch (channels)
        {
        case 1: format = VK_FORMAT_R8_SRGB;         bytesPerPixel = 1; break;
        case 2: format = VK_FORMAT_R8G8_SRGB;       bytesPerPixel = 2; break;
        case 3: format = VK_FORMAT_R8G8B8_SRGB;     bytesPerPixel = 3; break;
        default: format = VK_FORMAT_R8G8B8A8_SRGB;  bytesPerPixel = 4; break;
        }
    }

    VulkanTexture::VulkanTexture(const std::string& filePath, bool isHDR)
    {
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
            // 传入 0 让 stb_image 检测实际通道数
            unsigned char* data = stbi_load(filePath.c_str(), &w, &h, &channels, 0);
            if (!data)
            {
                AZ_CORE_ERROR("Failed to load image: {0}", filePath);
                return;
            }

            VkFormat format;
            uint32_t bytesPerPixel;
            ResolveChannelFormat(channels, format, bytesPerPixel);

            CreateFromData(data, static_cast<uint32_t>(w), static_cast<uint32_t>(h), format, bytesPerPixel);
            stbi_image_free(data);
        }
    }

    VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, void *data)
    {
        CreateFromData(data, width, height, VK_FORMAT_R8G8B8A8_SRGB, 4);
    }

    VulkanTexture::~VulkanTexture()
    {
        Ref<VulkanContext> ctx = VulkanContextManager::GetContext();

        if (m_DescriptorSet != VK_NULL_HANDLE)
        {
            vkFreeDescriptorSets(ctx->Device, ctx->TextureDescriptorPool, 1, &m_DescriptorSet);
        }

        if (m_Sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(ctx->Device, m_Sampler, nullptr);
        }

        if (m_ImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(ctx->Device, m_ImageView, nullptr);
        }

        if (m_Image != VK_NULL_HANDLE)
        {
            vmaDestroyImage(ctx->Allocator, m_Image, m_Allocation);
        }
    }

    void* VulkanTexture::GetHandle() const
    {
        return reinterpret_cast<void*>(m_DescriptorSet);
    }

    void VulkanTexture::Bind(const VkCommandBuffer& cmd, VkPipelineLayout pipelineLayout) const
    {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout, 1, 1, &m_DescriptorSet, 0, nullptr);
    }

    void VulkanTexture::CreateFromData(void* data, uint32_t width, uint32_t height, VkFormat format, uint32_t bytesPerPixel)
    {
        Ref<VulkanContext> ctx = VulkanContextManager::GetContext();

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

        VkResult result = vmaCreateImage(ctx->Allocator, &imageInfo, &allocInfo,
            &m_Image, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            AZ_CORE_ERROR("Failed to create image with VMA.");
            return;
        }

        // 2. staging buffer 拷贝像素数据
        VkDeviceSize imageSize = static_cast<VkDeviceSize>(width) * height * bytesPerPixel;
        VulkanStagingBuffer staging(ctx, static_cast<uint32_t>(imageSize));
        staging.Upload(data, static_cast<uint32_t>(imageSize));

        // 3. 临时 command buffer：layout transition + copy + transition
        VulkanCommandBuffer cmdBuffer(ctx);
        const VkCommandBuffer& cmd = cmdBuffer.Get();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &beginInfo);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_Image;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkBufferImageCopy region{};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent = { width, height, 1 };
        vkCmdCopyBufferToImage(cmd, staging.GetBuffer(), m_Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkEndCommandBuffer(cmd);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VkFence fence;
        vkCreateFence(ctx->Device, &fenceInfo, nullptr, &fence);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        vkQueueSubmit(ctx->GraphicsQueue, 1, &submitInfo, fence);
        vkWaitForFences(ctx->Device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(ctx->Device, fence, nullptr);

        // 4. ImageView
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(ctx->Device, &viewInfo, nullptr, &m_ImageView);

        // 5. Sampler
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkCreateSampler(ctx->Device, &samplerInfo, nullptr, &m_Sampler);

        // 6. Descriptor set（set 1, binding 0 = combined image sampler）
        VkDescriptorSetAllocateInfo setAllocInfo{};
        setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setAllocInfo.descriptorPool = ctx->TextureDescriptorPool;
        setAllocInfo.descriptorSetCount = 1;
        setAllocInfo.pSetLayouts = &ctx->TextureSetLayout;
        vkAllocateDescriptorSets(ctx->Device, &setAllocInfo, &m_DescriptorSet);

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
        vkUpdateDescriptorSets(ctx->Device, 1, &write, 0, nullptr);
    }
}
