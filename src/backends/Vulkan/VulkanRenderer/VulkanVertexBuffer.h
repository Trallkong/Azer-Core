#pragma once

#include "Base.h"
#include <vector>

#include "vulkan/vulkan.h"

#include "Mesh2D.h"
#include "vk_mem_alloc.h"

namespace Azer {

    class VulkanVertexBuffer 
    {
    public:
        VulkanVertexBuffer(uint32_t size);
        ~VulkanVertexBuffer();

        void Upload(const Vertices& vertices);
        void Bind(const VkCommandBuffer& cmd);

    private:
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
    };
}
