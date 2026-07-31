#pragma once

#include "Base.h"
#include <vector>

#include "vulkan/vulkan.h"

#include "Mesh2D.h"
#include "vk_mem_alloc.h"

namespace Azer {

    class VulkanIndexBuffer
    {
    public:
        VulkanIndexBuffer(uint32_t size);
        ~VulkanIndexBuffer();

        void Upload(const Indices& indices);
        void Bind(const VkCommandBuffer& cmd);

        uint32_t GetCount() const { return m_Count; }
    private:
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
        uint32_t m_Count = 0;
    };
}
