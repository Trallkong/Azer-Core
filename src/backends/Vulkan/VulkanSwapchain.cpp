#include "azpch.h"
#include "VulkanSwapchain.h"

#include "VulkanContextManager.h"

namespace Azer {

    VulkanSwapchain::VulkanSwapchain(Window *window)
        : m_Window(window)
    {
        m_SwapchainImageExtent = ChooseSwapchainExtent();
        m_Swapchain = CreateSwapchainInternal(VK_NULL_HANDLE, m_SwapchainImageExtent);
        RetrieveSwapchainImages();
        CreateSwapchainImageViews();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();
        DestroySwapchainImageViews();
        vkDestroySwapchainKHR(ctx.Device, m_Swapchain, nullptr);
    }

    void VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height)
    {
        vkDeviceWaitIdle(VulkanContextManager::GetContext().Device);

        DestroySwapchainImageViews();

        m_SwapchainImageExtent = { width, height };
        m_Swapchain = CreateSwapchainInternal(m_Swapchain, m_SwapchainImageExtent);

        RetrieveSwapchainImages();
        CreateSwapchainImageViews();
    }

    VkExtent2D VulkanSwapchain::ChooseSwapchainExtent()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            VulkanContextManager::GetContext().PhysicalDevice, 
            VulkanContextManager::GetContext().Surface, 
            &surfaceCapabilities
        );

        if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
            return surfaceCapabilities.currentExtent;
        }

        int width, height;
        width = m_Window->GetWindowSize().width;
        height = m_Window->GetWindowSize().height;
        
        uint32_t c_width = std::clamp<uint32_t>(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t c_height = std::clamp<uint32_t>(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        
        return { c_width, c_height };
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapchainFormat()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.PhysicalDevice, ctx.Surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.PhysicalDevice, ctx.Surface, &formatCount, surfaceFormats.data());
        
        for (const auto& format : surfaceFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                AZ_CORE_DEBUG("Using VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR for swapchain");
                return format;
            }
        }
        
        AZ_CORE_DEBUG("Using default swapchain format");
        return surfaceFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChooseSwapchainPresentMode()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.PhysicalDevice, ctx.Surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.PhysicalDevice, ctx.Surface, &presentModeCount, presentModes.data());
        
        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_FIFO_KHR) {
                AZ_CORE_DEBUG("Using VK_PRESENT_MODE_FIFO_KHR for swapchain");
                return mode;
            }
        }
        
        AZ_CORE_DEBUG("Using default present mode for swapchain");
        return presentModes[0];
    }

    VkSwapchainKHR VulkanSwapchain::CreateSwapchainInternal(VkSwapchainKHR oldSwapchain, VkExtent2D extent)
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        m_SwapchainSurfaceFormat = ChooseSwapchainFormat();
        m_SwapchainPresentMode = ChooseSwapchainPresentMode();

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.PhysicalDevice, ctx.Surface, &surfaceCapabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = ctx.Surface;
        swapchainCreateInfo.minImageCount = 2; // 双缓冲
        swapchainCreateInfo.imageFormat = m_SwapchainSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = m_SwapchainSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.presentMode = m_SwapchainPresentMode;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = oldSwapchain;

        VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
        VkResult result = vkCreateSwapchainKHR(ctx.Device, &swapchainCreateInfo, nullptr, &newSwapchain);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create swapchain");

        // 新交换链就绪后才销毁旧交换链，避免空档期
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(ctx.Device, oldSwapchain, nullptr);
        }

        return newSwapchain;
    }

    void VulkanSwapchain::RetrieveSwapchainImages()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(ctx.Device, m_Swapchain, &imageCount, nullptr);
        m_Images.clear();
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(ctx.Device, m_Swapchain, &imageCount, m_Images.data());
    }

    void VulkanSwapchain::CreateSwapchainImageViews()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        DestroySwapchainImageViews();
        m_ImageViews.resize(m_Images.size());

        for (uint32_t i = 0; i < m_Images.size(); ++i) {
            VkImageViewCreateInfo viewCreateInfo{};
            viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCreateInfo.image = m_Images[i];
            viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewCreateInfo.format = m_SwapchainSurfaceFormat.format;
            viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCreateInfo.subresourceRange.baseMipLevel = 0;
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseArrayLayer = 0;
            viewCreateInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(ctx.Device, &viewCreateInfo, nullptr, &m_ImageViews[i]);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create image views for swapchain images");
        }
    }

    void VulkanSwapchain::DestroySwapchainImageViews()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        for (auto& iv : m_ImageViews)
        {
            if (iv != VK_NULL_HANDLE)
            {
                vkDestroyImageView(ctx.Device, iv, nullptr);
                iv = VK_NULL_HANDLE;
            }
        }
        m_ImageViews.clear();
    }
}
