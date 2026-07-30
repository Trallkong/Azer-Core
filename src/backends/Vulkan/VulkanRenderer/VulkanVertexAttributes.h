#pragma once

#include "Base.h"
#include <vector>
#include <string>

#include "vulkan/vulkan.h"

namespace Azer {

    enum class VulkanVertexAttributeType {
        Mat2, Mat3, Mat4,
        Uint8, Uint16, Uint32, Uint64,
        Int8, Int16, Int32, Int64,
        Float, Float2, Float3, Float4,
    };

    inline uint32_t GetVertexAttributeBytes(VulkanVertexAttributeType type)
    {
        switch (type)
        {
            case VulkanVertexAttributeType::Mat2: return 4 * 2 * 2;
            case VulkanVertexAttributeType::Mat3: return 4 * 3 * 3;
            case VulkanVertexAttributeType::Mat4: return 4 * 4 * 4;
            case VulkanVertexAttributeType::Float: return 4 * 1;
            case VulkanVertexAttributeType::Float2: return 4 * 2;
            case VulkanVertexAttributeType::Float3: return 4 * 3;
            case VulkanVertexAttributeType::Float4: return 4 * 4;
            case VulkanVertexAttributeType::Uint8: return 1;
            case VulkanVertexAttributeType::Uint16: return 2;
            case VulkanVertexAttributeType::Uint32: return 4;
            case VulkanVertexAttributeType::Uint64: return 8;
            case VulkanVertexAttributeType::Int8: return 1;
            case VulkanVertexAttributeType::Int16: return 2;
            case VulkanVertexAttributeType::Int32: return 4;
            case VulkanVertexAttributeType::Int64: return 8;
        }
    }

    inline VkFormat GetVertexAttributeVkFormat(VulkanVertexAttributeType type)
    {
        switch (type)
        {
            case VulkanVertexAttributeType::Float: return VkFormat::VK_FORMAT_R32_SFLOAT;
            case VulkanVertexAttributeType::Float2: return VkFormat::VK_FORMAT_R32G32_SFLOAT;
            case VulkanVertexAttributeType::Float3: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            case VulkanVertexAttributeType::Float4: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
            case VulkanVertexAttributeType::Uint32: return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
        }
    }

    struct VulkanVertexAtrribute
    {
        VulkanVertexAttributeType type;
        std::string name;
    };

    class VulkanVertexAttributes 
    {
    public:
        VulkanVertexAttributes(std::vector<VulkanVertexAtrribute> attributes)
        {
            uint32_t offset = 0;
            for (int i = 0; i < attributes.size(); i++)
            {
                VkVertexInputAttributeDescription desc;
                desc.location = i;
                desc.binding = 0;
                desc.format = GetVertexAttributeVkFormat(attributes[i].type);
                desc.offset = offset;

                m_Descriptions.push_back(desc);

                offset += GetVertexAttributeBytes(attributes[i].type);
            }

            m_BindingDesc.binding = 0;
            m_BindingDesc.stride = offset;
            m_BindingDesc.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        }
        ~VulkanVertexAttributes() = default;

        const std::vector<VkVertexInputAttributeDescription>& GetDescriptions() const { return m_Descriptions; }
        const VkVertexInputBindingDescription& GetBindingDesc() const { return m_BindingDesc; }

    private:
        std::vector<VkVertexInputAttributeDescription> m_Descriptions;
        VkVertexInputBindingDescription m_BindingDesc;
    };
}