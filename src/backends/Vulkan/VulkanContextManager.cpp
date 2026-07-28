#include "azpch.h"
#include "VulkanContextManager.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

namespace Azer {

    void VulkanContextManager::Init(Window* window)
    {
        m_Window = window;

        m_Context = CreateRef<VulkanContext>();

        // Initialize Vulkan instance
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Azer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Azer";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

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
        #ifdef AZ_ENABLE_DEBUG
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        ValidateLayer(validationLayerName);
        #endif

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionCount;
        createInfo.ppEnabledExtensionNames = requiredExtensions;

        #ifdef AZ_ENABLE_DEBUG
        createInfo.enabledLayerCount = 1;
        const char* validationLayers[] = { validationLayerName };
        createInfo.ppEnabledLayerNames = validationLayers;
        #else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        #endif

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Context->Instance);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance");

        createSurface();
        choosePhysicalDevice();
        validateDeviceExtensions();
        createLogicalDevice();

        createSwapchain();
        createSwapchainImageViews();
        createMemAllocator();

        createMyDescriptorPool();
        createImGuiDescriptorPool();
        createCommandPool();
    }

    void VulkanContextManager::Shutdown()
    {
        // 1. 等待设备空闲
        if (m_Context->Device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(m_Context->Device);
        }

        // 2. 销毁命令池（会隐式销毁所有命令缓冲区）
        if (m_Context->cmdPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_Context->Device, m_Context->cmdPool, nullptr);
            m_Context->cmdPool = VK_NULL_HANDLE;
        }

        // 3. 销毁描述符池
        if (m_Context->ImGuiDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_Context->Device, m_Context->ImGuiDescriptorPool, nullptr);
            m_Context->ImGuiDescriptorPool = VK_NULL_HANDLE;
        }

        // 4. 销毁你的自定义描述符池（如果有）
        if (m_Context->MyDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_Context->Device, m_Context->MyDescriptorPool, nullptr);
        }

        // 5. 销毁 VMA 分配器
        if (m_Context->Allocator != VK_NULL_HANDLE) {
            vmaDestroyAllocator(m_Context->Allocator);
            m_Context->Allocator = VK_NULL_HANDLE;
        }

        // 6. 销毁交换链图像视图
        for (auto& imageView : m_Context->SwapchainImageViews) {
            if (imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_Context->Device, imageView, nullptr);
                imageView = VK_NULL_HANDLE;
            }
        }
        m_Context->SwapchainImageViews.clear();

        // 7. 销毁交换链
        if (m_Context->Swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_Context->Device, m_Context->Swapchain, nullptr);
            m_Context->Swapchain = VK_NULL_HANDLE;
        }

        // 8. 销毁表面
        if (m_Context->Surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(m_Context->Instance, m_Context->Surface, nullptr);
            m_Context->Surface = VK_NULL_HANDLE;
        }

        // 9. 销毁逻辑设备
        if (m_Context->Device != VK_NULL_HANDLE) {
            vkDestroyDevice(m_Context->Device, nullptr);
            m_Context->Device = VK_NULL_HANDLE;
        }

        // 10. 销毁实例
        if (m_Context->Instance != VK_NULL_HANDLE) {
            vkDestroyInstance(m_Context->Instance, nullptr);
            m_Context->Instance = VK_NULL_HANDLE;
        }
    }

    void VulkanContextManager::ReCreateSwapchain(uint32_t width, uint32_t height)
    {
        if (m_Context->Swapchain != VK_NULL_HANDLE) 
        {
            vkDeviceWaitIdle(m_Context->Device);
            vkDestroySwapchainKHR(m_Context->Device, m_Context->Swapchain, nullptr);
        }

        VkSurfaceFormatKHR surfaceFormat = chooseSwapchainFormat();
        VkPresentModeKHR presentMode = chooseSwapchainPresentMode();
        m_Context->SwapchainImageExtent = { width, height };

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_Context->Surface;
        swapchainCreateInfo.minImageCount = 2; // 双缓冲
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = m_Context->SwapchainImageExtent;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->PhysicalDevice, m_Context->Surface, &surfaceCapabilities);

        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        VkResult result = vkCreateSwapchainKHR(m_Context->Device, &swapchainCreateInfo, nullptr, &m_Context->Swapchain);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create swapchain");

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, nullptr);
        m_Context->SwapchainImages.clear();
        m_Context->SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, m_Context->SwapchainImages.data());
    
        createSwapchainImageViews();
    }

    std::vector<VkExtensionProperties> VulkanContextManager::EnumerateInstanceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (extensionCount == 0) {
            AZ_CORE_ERROR("No Vulkan instance extensions found");
            return std::vector<VkExtensionProperties>();
        }

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        AZ_CORE_DEBUG("Available Vulkan Instance Extensions:");
        for (const auto& ext : extensions) {
            AZ_CORE_DEBUG("  {} (version {})", ext.extensionName, ext.specVersion);
        }

        return extensions;
    }
    
    std::vector<VkLayerProperties> VulkanContextManager::EnumerateInstanceLayers()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (layerCount == 0) {
            AZ_CORE_ERROR("No Vulkan instance layers found");
            return std::vector<VkLayerProperties>();
        }

        std::vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        AZ_CORE_DEBUG("Available Vulkan Instance Layers:");
        for (const auto& layer : layers) {
            AZ_CORE_DEBUG("  {0} (version {1})", layer.layerName, layer.specVersion);
        }

        return layers;
    }

    void VulkanContextManager::ValidateLayer(const char *layerName)
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

    void VulkanContextManager::choosePhysicalDevice()
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_Context->Instance, &physicalDeviceCount, nullptr);
        AZ_ASSERT(physicalDeviceCount > 0, "Failed to find GPUs with Vulkan support");

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_Context->Instance, &physicalDeviceCount, physicalDevices.data());

        for (const auto& device : physicalDevices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            // 检查队列族支持
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            bool foundGraphicsQueue = false;
            bool foundPresentQueue = false;
            
            uint32_t graphicsQueueIndex = 0;
            uint32_t presentQueueIndex = 0;

            for (uint32_t i = 0; i < queueFamilyCount; i++) 
            {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    graphicsQueueIndex = i;
                    foundGraphicsQueue = true;
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Context->Surface, &presentSupport);
                if (presentSupport)
                {
                    presentQueueIndex = i;
                    foundPresentQueue = true;
                }

                if (foundGraphicsQueue && foundPresentQueue)
                {
                    break;
                }
            }


            // 检查交换链扩展支持
            uint32_t extensioncount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, nullptr);
            std::vector<VkExtensionProperties> availiableExtensions(extensioncount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, availiableExtensions.data());

            bool swapchainSupported = false;
            for (const auto& ext : availiableExtensions) 
            {
                if (strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
                {
                    swapchainSupported = true;
                    break;
                }
            }

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                deviceFeatures.geometryShader && foundGraphicsQueue && foundPresentQueue && swapchainSupported
            ) {
                m_Context->PhysicalDevice = device;
                m_Context->QueueFamilyIndex = graphicsQueueIndex;
                m_Context->PresentQueueFamilyIndex = presentQueueIndex;

                AZ_CORE_DEBUG("Selected GPU: {0}", deviceProperties.deviceName);
                AZ_CORE_DEBUG("Graphics Queue: {0}, Present Queue: {1}", graphicsQueueIndex, presentQueueIndex);
                return;
            }
        }

        for (const auto& device : physicalDevices)
        {
            // 兜底
        }

        AZ_ASSERT(false, "Failed to find a suitable Vulkan physical device!");
    }

    void VulkanContextManager::validateDeviceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_Context->PhysicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_Context->PhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());

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

    void VulkanContextManager::createLogicalDevice()
    {
        float queuePriority = 1.0f;

        // ⭐ 处理图形和呈现队列（可能相同也可能不同）
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            m_Context->QueueFamilyIndex,
            m_Context->PresentQueueFamilyIndex
        };

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // ⭐ Vulkan 1.3 特性（动态渲染、同步2等）
        VkPhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13Features.dynamicRendering = VK_TRUE;    // 动态渲染
        vulkan13Features.synchronization2 = VK_TRUE;    // 同步2

        // Vulkan 1.2 特性（可选）
        VkPhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.bufferDeviceAddress = VK_TRUE;
        
        // 链接特性链
        vulkan12Features.pNext = &vulkan13Features;

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.fillModeNonSolid = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &vulkan12Features;  // 链接到 1.2 特性（包含 1.3）
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = m_RequiredDeviceExtensions.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledLayerCount = 0;  // 设备层已弃用

        VkResult result = vkCreateDevice(m_Context->PhysicalDevice, &deviceCreateInfo, nullptr, &m_Context->Device);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

        // ⭐ 获取队列句柄
        vkGetDeviceQueue(m_Context->Device, m_Context->QueueFamilyIndex, 0, &m_Context->GraphicsQueue);
        vkGetDeviceQueue(m_Context->Device, m_Context->PresentQueueFamilyIndex, 0, &m_Context->PresentQueue);
    }

    void VulkanContextManager::createSurface()
    {
        SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(m_Window->GetHandle()), m_Context->Instance, nullptr, &m_Context->Surface);
    }

    void VulkanContextManager::createSwapchain()
    {
        VkSurfaceFormatKHR surfaceFormat = chooseSwapchainFormat();
        VkPresentModeKHR presentMode = chooseSwapchainPresentMode();
        chooseSwapchainExtent();

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_Context->Surface;
        swapchainCreateInfo.minImageCount = 2; // 双缓冲
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = m_Context->SwapchainImageExtent;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->PhysicalDevice, m_Context->Surface, &surfaceCapabilities);

        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        VkResult result = vkCreateSwapchainKHR(m_Context->Device, &swapchainCreateInfo, nullptr, &m_Context->Swapchain);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create swapchain");

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, nullptr);
        m_Context->SwapchainImages.clear();
        m_Context->SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, m_Context->SwapchainImages.data());
    }

    VkSurfaceFormatKHR VulkanContextManager::chooseSwapchainFormat()
    {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->PhysicalDevice, m_Context->Surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->PhysicalDevice, m_Context->Surface, &formatCount, surfaceFormats.data());
        
        for (const auto& format : surfaceFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                m_Context->SwapchainImageFormat = format.format;
                AZ_CORE_DEBUG("Using VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR for swapchain");
                return format;
            }
        }
        
        AZ_CORE_DEBUG("Using default swapchain format");
        m_Context->SwapchainImageFormat = surfaceFormats[0].format;
        return surfaceFormats[0];
    }   

    VkPresentModeKHR VulkanContextManager::chooseSwapchainPresentMode()
    {
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->PhysicalDevice, m_Context->Surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->PhysicalDevice, m_Context->Surface, &presentModeCount, presentModes.data());
        
        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_FIFO_KHR) {
                AZ_CORE_DEBUG("Using VK_PRESENT_MODE_FIFO_KHR for swapchain");
                return mode;
            }
        }
        
        AZ_CORE_DEBUG("Using default present mode for swapchain");
        return presentModes[0];
    }

    // 交换范围就是交换链图像的分辨率，几乎是始终与我们绘制窗口的分辨率完全相等像素
    void VulkanContextManager::chooseSwapchainExtent()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->PhysicalDevice, m_Context->Surface, &surfaceCapabilities);

        if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
            m_Context->SwapchainImageExtent = surfaceCapabilities.currentExtent;
            return;
        }

        int width, height;
        SDL_GetWindowSize(static_cast<SDL_Window*>(m_Window->GetHandle()), &width, &height);
        
        uint32_t c_width = std::clamp<uint32_t>(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t c_height = std::clamp<uint32_t>(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        
        m_Context->SwapchainImageExtent = { c_width, c_height };
    }

    void VulkanContextManager::createSwapchainImageViews()
    {
        for (auto& iv : m_Context->SwapchainImageViews) 
        {
            if (iv != VK_NULL_HANDLE)
                vkDestroyImageView(m_Context->Device, iv, nullptr);
        }
        m_Context->SwapchainImageViews.clear();
        m_Context->SwapchainImageViews.resize(m_Context->SwapchainImages.size());

        for (uint32_t i = 0; i < m_Context->SwapchainImages.size(); ++i) {
            VkImageViewCreateInfo viewCreateInfo{};
            viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewCreateInfo.image = m_Context->SwapchainImages[i];
            viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewCreateInfo.format = m_Context->SwapchainImageFormat;
            viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewCreateInfo.subresourceRange.baseMipLevel = 0;
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.baseArrayLayer = 0;
            viewCreateInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_Context->Device, &viewCreateInfo, nullptr, &m_Context->SwapchainImageViews[i]);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create image views for swapchain images");
        }
    }

    void VulkanContextManager::createMemAllocator()
    {
        VmaAllocatorCreateInfo info{};
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        info.device = m_Context->Device;
        info.instance = m_Context->Instance;
        info.physicalDevice = m_Context->PhysicalDevice;
        info.preferredLargeHeapBlockSize = 0;
        
        VkResult result = vmaCreateAllocator(&info, &m_Context->Allocator);
        if (result != VK_SUCCESS) 
        {
            AZ_CORE_ERROR("创建内存分配器失败");
        }
    }

    void VulkanContextManager::createMyDescriptorPool()
    {

    }

    void VulkanContextManager::createImGuiDescriptorPool()
    {
        // 使用 std::array 定义描述符池大小
        std::array<VkDescriptorPoolSize, 11> poolSizes = {{
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        }};

        uint32_t maxSets = 1000 * static_cast<uint32_t>(poolSizes.size());
        
        m_Context->ImGuiDescriptorPool = createDescriptorPool(
            m_Context->Device,
            poolSizes,
            maxSets
        );
    }

    void VulkanContextManager::createCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // 允许单独重置 CommandBuffer
        poolInfo.queueFamilyIndex = m_Context->QueueFamilyIndex;  // 图形队列族索引

        VkResult result = vkCreateCommandPool(m_Context->Device, &poolInfo, nullptr, &m_Context->cmdPool);
        if (result != VK_SUCCESS) {
            AZ_CORE_ERROR("创建CommandPool失败");
        }
    }

    template<size_t N>
    VkDescriptorPool VulkanContextManager::createDescriptorPool(
        VkDevice device,
        const std::array<VkDescriptorPoolSize, N>& poolSizes,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags flags
        )
    {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = flags;
        poolInfo.maxSets = maxSets;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        VkDescriptorPool pool = VK_NULL_HANDLE;
        VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
        if (result != VK_SUCCESS) {
            // 处理错误
            return VK_NULL_HANDLE;
        }
        return pool;
    }
}
