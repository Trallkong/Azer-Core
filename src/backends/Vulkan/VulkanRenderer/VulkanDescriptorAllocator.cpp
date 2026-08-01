#include "azpch.h"
#include "VulkanDescriptorAllocator.h"

namespace Azer {

    void VulkanDescriptorAllocator::Init(VkDevice device)
    {
        m_Device = device;
        m_CurrentPool = CreatePool();
        m_SetsInCurrentPool = 0;
    }

    void VulkanDescriptorAllocator::Shutdown()
    {
        if (m_CurrentPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_Device, m_CurrentPool, nullptr);
            m_CurrentPool = VK_NULL_HANDLE;
        }

        for (VkDescriptorPool pool : m_RetiredPools)
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_Device, pool, nullptr);
            }
        }
        m_RetiredPools.clear();
        m_SetsInCurrentPool = 0;
    }

    VkDescriptorPool VulkanDescriptorAllocator::CreatePool()
    {
        std::array<VkDescriptorPoolSize, 6> poolSizes = {{
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 128 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 256 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 32 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 64 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 16 },
        }};

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = kMaxSetsPerPool;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        VkDescriptorPool pool = VK_NULL_HANDLE;
        VkResult result = vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &pool);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool");
        return pool;
    }

    VkDescriptorSet VulkanDescriptorAllocator::Allocate(const VkDescriptorSetLayout &layout)
    {
        if (m_CurrentPool == VK_NULL_HANDLE || m_SetsInCurrentPool >= kMaxSetsPerPool)
        {
            if (m_CurrentPool != VK_NULL_HANDLE)
            {
                m_RetiredPools.push_back(m_CurrentPool);
            }
            m_CurrentPool = CreatePool();
            m_SetsInCurrentPool = 0;
        }

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_CurrentPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet set = VK_NULL_HANDLE;
        VkResult result = vkAllocateDescriptorSets(m_Device, &allocInfo, &set);

        // 单个池的描述符数量耗尽但 maxSets 未满：换新池重试
        if (result != VK_SUCCESS)
        {
            m_RetiredPools.push_back(m_CurrentPool);
            m_CurrentPool = CreatePool();
            m_SetsInCurrentPool = 0;

            allocInfo.descriptorPool = m_CurrentPool;
            result = vkAllocateDescriptorSets(m_Device, &allocInfo, &set);
        }

        AZ_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set");
        if (result != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }

        m_SetsInCurrentPool++;
        return set;
    }
}
