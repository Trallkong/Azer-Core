#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"
#include "base/Window.h"

#include "vk_mem_alloc.h"

namespace Azer {

    struct VulkanContext {
        VkInstance Instance;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        uint32_t QueueFamilyIndex;
        uint32_t PresentQueueFamilyIndex;
        VkQueue GraphicsQueue;
        VkQueue PresentQueue;
        VkSurfaceKHR Surface;
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;
        VkExtent2D SwapchainImageExtent;
        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VmaAllocator Allocator;
        VkDescriptorPool MyDescriptorPool;
        VkDescriptorPool ImGuiDescriptorPool;
        VkCommandPool cmdPool;
    };

    class VulkanRendererContext {
    public:
        VulkanRendererContext() = default;
        ~VulkanRendererContext() = default;

        void Init(Window* window);
        void Shutdown();

        inline VulkanContext& GetContext() { return m_Context; }
        inline const VulkanContext& GetContext() const { return m_Context; }

    private:
        Window* m_Window;
        VulkanContext m_Context;

        std::vector<const char*> m_RequiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        std::vector<VkExtensionProperties> EnumerateInstanceExtensions();
        std::vector<VkLayerProperties> EnumerateInstanceLayers();

        // 这不是验证层，这个函数被用来验证层的有效性
        void ValidateLayer(const char* layerName);

        void choosePhysicalDevice();
        void validateDeviceExtensions();
    
        void createLogicalDevice();
        void createSurface();
        void createSwapchain();

        VkSurfaceFormatKHR chooseSwapchainFormat();
        VkPresentModeKHR chooseSwapchainPresentMode();
        void chooseSwapchainExtent();

        void createSwapchainImageViews();
        void createMemAllocator();

        template<size_t N>
        VkDescriptorPool createDescriptorPool(
            VkDevice device,
            const std::array<VkDescriptorPoolSize, N>& poolSizes,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
        );

        void createMyDescriptorPool();
        void createImGuiDescriptorPool();
        void createCommandPool();
    };
}