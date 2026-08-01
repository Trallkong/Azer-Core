#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include <vector>

namespace Azer {

    // 通用描述符分配器：按任意 set layout 分配 descriptor set。
    // 池用满后自动创建新池，避免固定大小的描述符池在资源增加时耗尽。
    class VulkanDescriptorAllocator {
    public:
        VulkanDescriptorAllocator() = default;
        ~VulkanDescriptorAllocator() = default;

        void Init(VkDevice device);
        void Shutdown();

        VkDescriptorSet Allocate(const VkDescriptorSetLayout& layout);

    private:
        VkDescriptorPool CreatePool();

        VkDevice m_Device = VK_NULL_HANDLE;

        VkDescriptorPool m_CurrentPool = VK_NULL_HANDLE;
        uint32_t m_SetsInCurrentPool = 0;
        std::vector<VkDescriptorPool> m_RetiredPools;

        static constexpr uint32_t kMaxSetsPerPool = 1000;
    };
}
