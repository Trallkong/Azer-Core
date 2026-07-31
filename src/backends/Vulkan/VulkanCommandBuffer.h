#pragma once

#include "Base.h"
#include "vulkan/vulkan.h"

#include <functional>

namespace Azer {

    class VulkanCommandBuffer {
    public:
        VulkanCommandBuffer();
        ~VulkanCommandBuffer();

        // 记录并提交一次性命令（layout 转换、拷贝等），同步等待 GPU 完成
        static void SubmitSingleTime(const std::function<void(const VkCommandBuffer&)>& record);

        inline const VkCommandBuffer& Get() const { return m_Buffer; }
        inline VkCommandBuffer& Get() { return m_Buffer; }
    private:
        VkCommandBuffer m_Buffer;
    };
}
