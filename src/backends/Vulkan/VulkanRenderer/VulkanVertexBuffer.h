#pragma once

#include "Base.h"
#include <vector>

#include "vulkan/vulkan.h"

#include "VertexBuffer.h"
#include "Mesh2D.h"
#include "vk_mem_alloc.h"

namespace Azer {

    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(uint32_t size);
        ~VulkanVertexBuffer();

        void Upload(const Vertices& vertices) override;
        uint32_t GetSize() const override { return m_Size; }

        void Bind(const VkCommandBuffer& cmd);

    private:
        uint32_t m_Size = 0;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        void* m_MappedData = nullptr;
    };
}
