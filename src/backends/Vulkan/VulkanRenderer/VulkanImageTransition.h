#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

namespace Azer {

    // 图像布局转换封装：所有 barrier 本质都是"把图像从一种布局切到另一种布局"，
    // 封装后调用方不用再记 stage/access 的搭配。常见的几种转换已配好参数。
    class VulkanImageTransition {
    public:
        // 通用：手动指定所有同步参数
        static void Transition(
            const VkCommandBuffer& cmd,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageAspectFlags aspectMask,
            VkPipelineStageFlags srcStage,
            VkPipelineStageFlags dstStage,
            VkAccessFlags srcAccess,
            VkAccessFlags dstAccess)
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcAccessMask = srcAccess;
            barrier.dstAccessMask = dstAccess;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = aspectMask;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        // 交换链颜色图：开始渲染前（丢弃旧内容）
        static void ToColorAttachment(const VkCommandBuffer& cmd, VkImage image)
        {
            Transition(cmd, image,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        }

        // 交换链颜色图：渲染完交给 present
        static void ToPresent(const VkCommandBuffer& cmd, VkImage image)
        {
            Transition(cmd, image,
                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0);
        }

        // 深度图：创建后一次性转换（丢弃旧内容）
        static void ToDepthAttachment(const VkCommandBuffer& cmd, VkImage image)
        {
            Transition(cmd, image,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_ASPECT_DEPTH_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
        }

        // 纹理上传：接收 vkCmdCopyBufferToImage
        static void ToTransferDst(const VkCommandBuffer& cmd, VkImage image)
        {
            Transition(cmd, image,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                0, VK_ACCESS_TRANSFER_WRITE_BIT);
        }

        // 纹理上传完成后转成 shader 采样布局
        static void ToShaderRead(const VkCommandBuffer& cmd, VkImage image, VkImageLayout oldLayout)
        {
            Transition(cmd, image,
                oldLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
        }
    };
}
