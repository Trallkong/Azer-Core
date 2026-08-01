#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include <vector>

namespace Azer {

    class VulkanShader;
    class VulkanTexture;

    // 后端内部描述符集：从 shader 的某个 set layout 分配，把 buffer / 纹理写进对应 binding。
    // 绑定类型（UBO / 采样器）从 shader 反射信息查得，无需调用方指定。
    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet(const VulkanShader* shader, uint32_t set);
        ~VulkanDescriptorSet();

        void SetBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size);
        void SetTexture(uint32_t binding, const VulkanTexture* texture);
        void Update();

        inline VkDescriptorSet GetHandle() const { return m_DescriptorSet; }

    private:
        struct Binding {
            uint32_t BindingNumber = 0;
            VkDescriptorType Type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            VkBuffer Buffer = VK_NULL_HANDLE;
            VkDeviceSize BufferSize = 0;
            const VulkanTexture* Texture = nullptr;
        };

        VkDescriptorType GetBindingType(uint32_t binding) const;

        const VulkanShader* m_Shader = nullptr;
        uint32_t m_Set = 0;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        std::vector<Binding> m_Bindings;
    };
}
