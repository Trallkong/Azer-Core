#include "azpch.h"
#include "VulkanRendererContext.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

namespace Azer {

    VulkanRendererContext::~VulkanRendererContext()
    {
    }

    void VulkanRendererContext::Init(Window* window)
    {
        m_Window = window;

        // Initialize Vulkan instance
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Azer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Azer";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // 获取实例层扩展，与SDL_Vulkan_GetInstanceExtensions()返回的扩展列表进行比较，确保所有必需的扩展都可用
        uint32_t extensionCount = 0;
        const char* const* requiredExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        std::vector<VkExtensionProperties> instanceExtensions = EnumerateInstanceExtensions();
        for (uint32_t i = 0; i < extensionCount; ++i) {
            bool found = false;
            for (const auto& ext : instanceExtensions) {
                if (strcmp(requiredExtensions[i], ext.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            AZ_ASSERT(found, "Required Vulkan extension not found: %s", requiredExtensions[i]);
        }


        // 验证 验证层的有效性
        #ifdef AZ_DEBUG
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        ValidateLayer(validationLayerName);
        #endif

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionCount;
        createInfo.ppEnabledExtensionNames = requiredExtensions;

        #ifdef AZ_DEBUG
        createInfo.enabledLayerCount = 1;
        const char* validationLayers[] = { validationLayerName };
        createInfo.ppEnabledLayerNames = validationLayers;
        #else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        #endif

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Context.Instance);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance");

        choosePhysicalDevice();
        validateDeviceExtensions();
        validateDeviceFeatures();
        createLogicalDevice();
        createSurface();
        createSwapchain();
        createSwapchainImageViews();
    }

    std::vector<VkExtensionProperties> VulkanRendererContext::EnumerateInstanceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (extensionCount == 0) {
            AZ_ERROR("No Vulkan instance extensions found");
            return std::vector<VkExtensionProperties>();
        }

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        AZ_INFO("Available Vulkan Instance Extensions:");
        for (const auto& ext : extensions) {
            AZ_INFO("  %s (version %u)", ext.extensionName, ext.specVersion);
        }

        return extensions;
    }
    
    std::vector<VkLayerProperties> VulkanRendererContext::EnumerateInstanceLayers()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (layerCount == 0) {
            AZ_ERROR("No Vulkan instance layers found");
            return std::vector<VkLayerProperties>();
        }

        std::vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        AZ_INFO("Available Vulkan Instance Layers:");
        for (const auto& layer : layers) {
            AZ_INFO("  %s (version %u)", layer.layerName, layer.specVersion);
        }

        return layers;
    }

    void VulkanRendererContext::ValidateLayer(const char *layerName)
    {
        std::vector<VkLayerProperties> layers = EnumerateInstanceLayers();
        bool found = false;
        for (const auto& layer : layers) {
            if (strcmp(layer.layerName, layerName) == 0) {
                found = true;
                break;
            }
        }
        AZ_ASSERT(found, "Vulkan layer not found: %s", layerName);
    }

    void VulkanRendererContext::choosePhysicalDevice()
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_Context.Instance, &physicalDeviceCount, nullptr);
        AZ_ASSERT(physicalDeviceCount > 0, "Failed to find GPUs with Vulkan support");

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_Context.Instance, &physicalDeviceCount, physicalDevices.data());

        for (const auto& device : physicalDevices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            AZ_INFO("Found GPU: %s (API version %u)", deviceProperties.deviceName, deviceProperties.apiVersion);

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader) {
                m_Context.PhysicalDevice = device;
                AZ_INFO("Selected GPU: %s", deviceProperties.deviceName);
                return;
            }
        }
    }

    void VulkanRendererContext::validateDeviceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_Context.PhysicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_Context.PhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());

        for (const char* requiredExtension : m_RequiredDeviceExtensions) {
            bool found = false;
            for (const auto& ext : deviceExtensions) {
                if (strcmp(requiredExtension, ext.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            AZ_ASSERT(found, "Required Vulkan device extension not found: %s", requiredExtension);
        }
    }

    void VulkanRendererContext::validateDeviceFeatures()
    {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(m_Context.PhysicalDevice, &deviceFeatures);

        AZ_ASSERT(deviceFeatures.geometryShader, "Required feature 'geometryShader' is not supported by the selected physical device");
    }

    void VulkanRendererContext::createLogicalDevice()
    {
        uint32_t queueFamilyPropertyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_Context.PhysicalDevice, &queueFamilyPropertyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_Context.PhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

        for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                float queuePriority = 1.0f;
                queueCreateInfo.pQueuePriorities = &queuePriority;

                VkDeviceCreateInfo deviceCreateInfo{};
                deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                deviceCreateInfo.queueCreateInfoCount = 1;
                deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
                deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredDeviceExtensions.size());
                deviceCreateInfo.ppEnabledExtensionNames = m_RequiredDeviceExtensions.data();

                VkResult result = vkCreateDevice(m_Context.PhysicalDevice, &deviceCreateInfo, nullptr, &m_Context.Device);
                AZ_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

                vkGetDeviceQueue(m_Context.Device, i, 0, &m_Context.GraphicsQueue);
                return;
            }
        }
        AZ_ERROR("Failed to find a suitable queue family for graphics");
    }

    void VulkanRendererContext::createSurface()
    {
        SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(m_Window->GetHandle()), m_Context.Instance, nullptr, &m_Context.Surface);
    }

    void VulkanRendererContext::createSwapchain()
    {
        VkSurfaceFormatKHR surfaceFormat = chooseSwapchainFormat();
        VkPresentModeKHR presentMode = chooseSwapchainPresentMode();
        VkExtent2D extent = chooseSwapchainExtent();

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_Context.Surface;
        swapchainCreateInfo.minImageCount = 2; // 双缓冲
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context.PhysicalDevice, m_Context.Surface, &surfaceCapabilities);

        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        VkResult result = vkCreateSwapchainKHR(m_Context.Device, &swapchainCreateInfo, nullptr, &m_Context.Swapchain);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create swapchain");

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Context.Device, m_Context.Swapchain, &imageCount, nullptr);
        m_Context.SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Context.Device, m_Context.Swapchain, &imageCount, m_Context.SwapchainImages.data());
    }

    VkSurfaceFormatKHR VulkanRendererContext::chooseSwapchainFormat()
    {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context.PhysicalDevice, m_Context.Surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context.PhysicalDevice, m_Context.Surface, &formatCount, surfaceFormats.data());
        
        for (const auto& format : surfaceFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                m_Context.SwapchainImageFormat = format.format;
                AZ_INFO("Using VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR for swapchain");
                return format;
            }
        }
        
        AZ_INFO("Using default swapchain format");
        m_Context.SwapchainImageFormat = surfaceFormats[0].format;
        return surfaceFormats[0];
    }   

    VkPresentModeKHR VulkanRendererContext::chooseSwapchainPresentMode()
    {
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context.PhysicalDevice, m_Context.Surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context.PhysicalDevice, m_Context.Surface, &presentModeCount, presentModes.data());
        
        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_FIFO_KHR) {
                AZ_INFO("Using VK_PRESENT_MODE_FIFO_KHR for swapchain");
                return mode;
            }
        }
        
        AZ_INFO("Using default present mode for swapchain");
        return presentModes[0];
    }

    // 交换范围就是交换链图像的分辨率，几乎是始终与我们绘制窗口的分辨率完全相等像素
    VkExtent2D VulkanRendererContext::chooseSwapchainExtent()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context.PhysicalDevice, m_Context.Surface, &surfaceCapabilities);

        if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
            return surfaceCapabilities.currentExtent;
        }

        int width, height;
        SDL_GetWindowSize(static_cast<SDL_Window*>(m_Window->GetHandle()), &width, &height);

        return {
            std::clamp<uint32_t>(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
        };
    }

    void VulkanRendererContext::createSwapchainImageViews()
    {
        AZ_ASSERT(m_Context.SwapchainImages.empty(), "Swapchain images need to be empty");
        m_Context.SwapchainImageViews.resize(m_Context.SwapchainImages.size());

        for (uint32_t i = 0; i < m_Context.SwapchainImages.size(); ++i) {
            VkImageViewCreateInfo viewCreateInfo{};
            viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCreateInfo.image = m_Context.SwapchainImages[i];
            viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewCreateInfo.format = m_Context.SwapchainImageFormat;
            viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCreateInfo.subresourceRange.baseMipLevel = 0;
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseArrayLayer = 0;
            viewCreateInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_Context.Device, &viewCreateInfo, nullptr, &m_Context.SwapchainImageViews[i]);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create image views for swapchain images");
        }
    }
}
