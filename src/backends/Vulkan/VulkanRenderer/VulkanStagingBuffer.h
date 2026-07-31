#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include "vk_mem_alloc.h"

namespace Azer {

    class VulkanStagingBuffer
    {
    public:
        VulkanStagingBuffer(uint32_t size);
        ~VulkanStagingBuffer();

        void Upload(void* data, uint32_t size);

        const VkBuffer& GetBuffer() const { return m_Buffer; }
    private:
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
    };
}
