#include "azpch.h"
#include "VulkanDescriptorSet.h"

#include <algorithm>

#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanContextManager.h"

namespace Azer {

    VulkanDescriptorSet::VulkanDescriptorSet(const VulkanShader* shader, uint32_t set)
        : m_Shader(shader), m_Set(set)
    {
        VulkanContext& ctx = VulkanContextManager::GetContext();

        if (shader == nullptr)
        {
            return;
        }

        m_DescriptorSet = ctx.DescriptorAllocator.Allocate(shader->GetSetLayout(set));
    }

    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        // descriptor set 由 VulkanDescriptorAllocator 的池统一管理，无需手动释放
    }

    VkDescriptorType VulkanDescriptorSet::GetBindingType(uint32_t binding) const
    {
        if (m_Shader == nullptr)
        {
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }

        for (const auto& sb : m_Shader->GetSetBindings(m_Set))
        {
            if (sb.Binding == binding)
            {
                // 统一按动态 uniform buffer 处理（每绘制一个 offset）
                if (sb.Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                {
                    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                }
                return sb.Type;
            }
        }
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }

    void VulkanDescriptorSet::SetBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size)
    {
        VkDescriptorType type = GetBindingType(binding);
        if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM)
        {
            AZ_CORE_WARN("VulkanDescriptorSet: shader set {0} has no binding {1}", m_Set, binding);
            return;
        }

        m_Bindings.erase(
            std::remove_if(m_Bindings.begin(), m_Bindings.end(),
                [&](const Binding& x) { return x.BindingNumber == binding; }),
            m_Bindings.end());

        Binding b;
        b.BindingNumber = binding;
        b.Type = type;
        b.Buffer = buffer;
        b.BufferSize = size;
        m_Bindings.push_back(b);
    }

    void VulkanDescriptorSet::SetTexture(uint32_t binding, const VulkanTexture* texture)
    {
        VkDescriptorType type = GetBindingType(binding);
        if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM)
        {
            AZ_CORE_WARN("VulkanDescriptorSet: shader set {0} has no binding {1}", m_Set, binding);
            return;
        }

        m_Bindings.erase(
            std::remove_if(m_Bindings.begin(), m_Bindings.end(),
                [&](const Binding& x) { return x.BindingNumber == binding; }),
            m_Bindings.end());

        Binding b;
        b.BindingNumber = binding;
        b.Type = type;
        b.Texture = texture;
        m_Bindings.push_back(b);
    }

    void VulkanDescriptorSet::Update()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkWriteDescriptorSet> writes;

        // 预分配，避免 push_back 扩容使 pBufferInfo / pImageInfo 指针悬空
        bufferInfos.reserve(m_Bindings.size());
        imageInfos.reserve(m_Bindings.size());
        writes.reserve(m_Bindings.size());

        for (const auto& b : m_Bindings)
        {
            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_DescriptorSet;
            write.dstBinding = b.BindingNumber;
            write.descriptorCount = 1;
            write.descriptorType = b.Type;

            if (b.Buffer != VK_NULL_HANDLE)
            {
                VkDescriptorBufferInfo info{};
                info.buffer = b.Buffer;
                info.offset = 0;
                info.range = b.BufferSize;
                bufferInfos.push_back(info);
                write.pBufferInfo = &bufferInfos.back();
            }
            else if (b.Texture != nullptr)
            {
                VkDescriptorImageInfo info{};
                info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                info.imageView = b.Texture->GetImageView();
                info.sampler = b.Texture->GetSampler();
                imageInfos.push_back(info);
                write.pImageInfo = &imageInfos.back();
            }
            else
            {
                continue;
            }

            writes.push_back(write);
        }

        if (!writes.empty())
        {
            vkUpdateDescriptorSets(ctx.Device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
        }
    }
}
