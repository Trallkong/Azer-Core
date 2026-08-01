#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include "vk_mem_alloc.h"

namespace Azer {

    // 通用 uniform 缓冲（后端内部使用）：纯数据缓冲，不持有描述符集。
    // 描述符集由 VulkanDescriptorSet 负责把此缓冲写入 shader 的 set binding。
    class VulkanUniformBuffer
    {
    public:
        explicit VulkanUniformBuffer(uint32_t size);
        ~VulkanUniformBuffer();

        void SetData(const void* data, uint32_t size);
        // 写入指定偏移（动态 uniform 环形缓冲用）
        void SetData(const void* data, uint32_t size, VkDeviceSize offset);
        uint32_t GetSize() const { return m_Size; }

        inline VkBuffer GetBuffer() const { return m_Buffer; }

    private:
        uint32_t m_Size = 0;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
    };
}
