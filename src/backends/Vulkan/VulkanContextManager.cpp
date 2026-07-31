#include "azpch.h"
#include "VulkanContextManager.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

namespace Azer {

    VulkanContext* VulkanContextManager::s_Context = nullptr;

    void VulkanContextManager::Init(Window* window)
    {
        if (s_Context != nullptr)
        {
            AZ_CORE_WARN("VulkanContext already initialized, skipping Init");
            return;
        }

        m_Window = window;
        s_Context = new VulkanContext();

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

        VkResult result = vkCreateInstance(&createInfo, nullptr, &s_Context->Instance);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance");

        createSurface();
        choosePhysicalDevice();
        validateDeviceExtensions();
        createLogicalDevice();

        s_Context->Swapchain = CreateScope<VulkanSwapchain>(window);
        createMemAllocator();

        createMyDescriptorPool();
        createImGuiDescriptorPool();
        createTextureDescriptorPool();
        createDescriptorSetLayouts();
        createCommandPool();
    }

    void VulkanContextManager::Shutdown()
    {
        if (s_Context == nullptr)
        {
            AZ_CORE_WARN("VulkanContext not initialized, skipping Shutdown");
            return;
        }

        // 1. 等待设备空闲
        if (s_Context->Device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(s_Context->Device);
        }

        // 2. 销毁命令池（会隐式销毁所有命令缓冲区）
        if (s_Context->cmdPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(s_Context->Device, s_Context->cmdPool, nullptr);
            s_Context->cmdPool = VK_NULL_HANDLE;
        }

        // 3. 销毁描述符池
        if (s_Context->ImGuiDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(s_Context->Device, s_Context->ImGuiDescriptorPool, nullptr);
            s_Context->ImGuiDescriptorPool = VK_NULL_HANDLE;
        }

        // 4. 销毁你的自定义描述符池（如果有）
        if (s_Context->MyDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(s_Context->Device, s_Context->MyDescriptorPool, nullptr);
        }

        // 4.1 销毁纹理描述符池 + set layout
        if (s_Context->TextureDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(s_Context->Device, s_Context->TextureDescriptorPool, nullptr);
            s_Context->TextureDescriptorPool = VK_NULL_HANDLE;
        }
        if (s_Context->UboSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(s_Context->Device, s_Context->UboSetLayout, nullptr);
            s_Context->UboSetLayout = VK_NULL_HANDLE;
        }
        if (s_Context->TextureSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(s_Context->Device, s_Context->TextureSetLayout, nullptr);
            s_Context->TextureSetLayout = VK_NULL_HANDLE;
        }

        // 5. 销毁 VMA 分配器
        if (s_Context->Allocator != VK_NULL_HANDLE) {
            vmaDestroyAllocator(s_Context->Allocator);
            s_Context->Allocator = VK_NULL_HANDLE;
        }

        // 7. 销毁交换链
        s_Context->Swapchain.reset();

        // 8. 销毁表面
        if (s_Context->Surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(s_Context->Instance, s_Context->Surface, nullptr);
            s_Context->Surface = VK_NULL_HANDLE;
        }

        // 9. 销毁逻辑设备
        if (s_Context->Device != VK_NULL_HANDLE) {
            vkDestroyDevice(s_Context->Device, nullptr);
            s_Context->Device = VK_NULL_HANDLE;
        }

        // 10. 销毁实例
        if (s_Context->Instance != VK_NULL_HANDLE) {
            vkDestroyInstance(s_Context->Instance, nullptr);
            s_Context->Instance = VK_NULL_HANDLE;
        }

        delete s_Context;
        s_Context = nullptr;
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
        vkEnumeratePhysicalDevices(s_Context->Instance, &physicalDeviceCount, nullptr);
        AZ_ASSERT(physicalDeviceCount > 0, "Failed to find GPUs with Vulkan support");

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(s_Context->Instance, &physicalDeviceCount, physicalDevices.data());

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
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, s_Context->Surface, &presentSupport);
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
                s_Context->PhysicalDevice = device;
                s_Context->QueueFamilyIndex = graphicsQueueIndex;
                s_Context->PresentQueueFamilyIndex = presentQueueIndex;

                AZ_CORE_DEBUG("Selected GPU: {0}", deviceProperties.deviceName);
                AZ_CORE_DEBUG("Graphics Queue: {0}, Present Queue: {1}", graphicsQueueIndex, presentQueueIndex);
                return;
            }
        }

        AZ_ASSERT(false, "Failed to find a suitable Vulkan physical device!");
    }

    void VulkanContextManager::validateDeviceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(s_Context->PhysicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(s_Context->PhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());

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
            s_Context->QueueFamilyIndex,
            s_Context->PresentQueueFamilyIndex
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

        VkResult result = vkCreateDevice(s_Context->PhysicalDevice, &deviceCreateInfo, nullptr, &s_Context->Device);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

        // ⭐ 获取队列句柄
        vkGetDeviceQueue(s_Context->Device, s_Context->QueueFamilyIndex, 0, &s_Context->GraphicsQueue);
        vkGetDeviceQueue(s_Context->Device, s_Context->PresentQueueFamilyIndex, 0, &s_Context->PresentQueue);
    }

    void VulkanContextManager::createSurface()
    {
        SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(m_Window->GetHandle()), s_Context->Instance, nullptr, &s_Context->Surface);
    }

    void VulkanContextManager::createMemAllocator()
    {
        VmaAllocatorCreateInfo info{};
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        info.device = s_Context->Device;
        info.instance = s_Context->Instance;
        info.physicalDevice = s_Context->PhysicalDevice;
        info.preferredLargeHeapBlockSize = 0;
        
        VkResult result = vmaCreateAllocator(&info, &s_Context->Allocator);
        if (result != VK_SUCCESS) 
        {
            AZ_CORE_ERROR("创建内存分配器失败");
        }
    }

    void VulkanContextManager::createMyDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes = {{
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 10 }
        }};

        s_Context->MyDescriptorPool = createDescriptorPool(
            s_Context->Device,
            poolSizes,
            10
        );
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
        
        s_Context->ImGuiDescriptorPool = createDescriptorPool(
            s_Context->Device,
            poolSizes,
            maxSets
        );
    }

    void VulkanContextManager::createTextureDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 1> poolSizes = {{
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 }
        }};

        s_Context->TextureDescriptorPool = createDescriptorPool(
            s_Context->Device,
            poolSizes,
            100
        );
    }

    void VulkanContextManager::createDescriptorSetLayouts()
    {
        // set 0：UBO（顶点阶段）
        VkDescriptorSetLayoutBinding uboBinding{};
        uboBinding.binding = 0;
        uboBinding.descriptorCount = 1;
        uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
        uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uboLayoutInfo.bindingCount = 1;
        uboLayoutInfo.pBindings = &uboBinding;
        vkCreateDescriptorSetLayout(s_Context->Device, &uboLayoutInfo, nullptr, &s_Context->UboSetLayout);

        // set 1：纹理（片段阶段）
        VkDescriptorSetLayoutBinding texBinding{};
        texBinding.binding = 0;
        texBinding.descriptorCount = 1;
        texBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo texLayoutInfo{};
        texLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        texLayoutInfo.bindingCount = 1;
        texLayoutInfo.pBindings = &texBinding;
        vkCreateDescriptorSetLayout(s_Context->Device, &texLayoutInfo, nullptr, &s_Context->TextureSetLayout);
    }

    void VulkanContextManager::createCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // 允许单独重置 CommandBuffer
        poolInfo.queueFamilyIndex = s_Context->QueueFamilyIndex;  // 图形队列族索引

        VkResult result = vkCreateCommandPool(s_Context->Device, &poolInfo, nullptr, &s_Context->cmdPool);
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
