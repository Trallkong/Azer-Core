#pragma once

#include "Base.h"

#include "vulkan/vulkan.h"
#include <vector>

#include "Window.h"

namespace Azer {

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(Window* window);
        ~VulkanSwapchain();

        void RecreateSwapchain(uint32_t width, uint32_t height);

        inline const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain; }
        inline const VkExtent2D& GetExtent() const { return m_SwapchainImageExtent; }
        inline const std::vector<VkImage>& GetImage() const { return m_Images; }
        inline const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
        inline const VkFormat& GetFormat() const { return m_SwapchainSurfaceFormat.format; }

    private:
        Window* m_Window;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        VkExtent2D m_SwapchainImageExtent;
        VkSurfaceFormatKHR m_SwapchainSurfaceFormat;
        VkPresentModeKHR m_SwapchainPresentMode;

        VkExtent2D ChooseSwapchainExtent();
        VkSurfaceFormatKHR ChooseSwapchainFormat();
        VkPresentModeKHR ChooseSwapchainPresentMode();

        VkSwapchainKHR CreateSwapchainInternal(VkSwapchainKHR oldSwapchain, VkExtent2D extent);
        void RetrieveSwapchainImages();
        void CreateSwapchainImageViews();
        void DestroySwapchainImageViews();
    };
}