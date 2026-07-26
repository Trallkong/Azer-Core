#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"
#include "base/Window.h"

namespace Azer {

    struct VulkanContext {
        VkInstance Instance;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VkQueue GraphicsQueue;
        VkSurfaceKHR Surface;
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;
        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
    };

    class VulkanRendererContext {
    public:
        VulkanRendererContext() = default;
        ~VulkanRendererContext();

        void Init(Window* window);

        inline VulkanContext& GetContext() { return m_Context; }

    private:
        Window* m_Window;
        VulkanContext m_Context;

        std::vector<const char*> m_RequiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        std::vector<VkExtensionProperties> EnumerateInstanceExtensions();
        std::vector<VkLayerProperties> EnumerateInstanceLayers();

        // 这不是验证层，这个函数被用来验证层的有效性
        void ValidateLayer(const char* layerName);

        void choosePhysicalDevice();
        void validateDeviceExtensions();
        void validateDeviceFeatures(); // TODO：修改对显卡功能的需求，实际上应该在选择物理设备时就进行验证
    
        void createLogicalDevice();
        void createSurface();
        void createSwapchain();

        VkSurfaceFormatKHR chooseSwapchainFormat();
        VkPresentModeKHR chooseSwapchainPresentMode();
        VkExtent2D chooseSwapchainExtent();

        void createSwapchainImageViews();
    };
}