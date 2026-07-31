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
            default:
                AZ_CORE_ERROR("Unsupported vertex attribute type for byte size");
                return 0;
        }
    }

    inline VkFormat GetVertexAttributeVkFormat(VulkanVertexAttributeType type)
    {
        switch (type)
        {
            case VulkanVertexAttributeType::Float:  return VK_FORMAT_R32_SFLOAT;
            case VulkanVertexAttributeType::Float2: return VK_FORMAT_R32G32_SFLOAT;
            case VulkanVertexAttributeType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
            case VulkanVertexAttributeType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
            case VulkanVertexAttributeType::Int8:   return VK_FORMAT_R8_SINT;
            case VulkanVertexAttributeType::Int16:  return VK_FORMAT_R16_SINT;
            case VulkanVertexAttributeType::Int32:  return VK_FORMAT_R32_SINT;
            case VulkanVertexAttributeType::Int64:  return VK_FORMAT_R64_SINT;
            case VulkanVertexAttributeType::Uint8:  return VK_FORMAT_R8_UINT;
            case VulkanVertexAttributeType::Uint16: return VK_FORMAT_R16_UINT;
            case VulkanVertexAttributeType::Uint32: return VK_FORMAT_R32_UINT;
            case VulkanVertexAttributeType::Uint64: return VK_FORMAT_R64_UINT;
            case VulkanVertexAttributeType::Mat2:
            case VulkanVertexAttributeType::Mat3:
            case VulkanVertexAttributeType::Mat4:
            default:
                AZ_CORE_ERROR("Unsupported vertex attribute type for VkFormat mapping");
                return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
    }

    struct VulkanVertexAttribute
    {
        VulkanVertexAttributeType type;
        std::string name;
    };

    class VulkanVertexAttributes 
    {
    public:
        VulkanVertexAttributes(std::vector<VulkanVertexAttribute> attributes)
        {
            uint32_t offset = 0;
            for (size_t i = 0; i < attributes.size(); i++)
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