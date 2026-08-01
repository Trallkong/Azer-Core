#pragma once

#include "Base.h"
#include "Shader.h"

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include "vulkan/vulkan.h"

#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"

namespace Azer {

    enum class ShaderStage {
        VERTEX,
        FRAGMENT
    };

    // 从 .azshader 的 pipeline 段解析出的管线状态
    struct ShaderPipelineConfig {
        VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        bool DepthTestEnable = false;
        bool DepthWriteEnable = false;
        bool DepthClampEnable = false;
        bool BlendEnable = true;
        VkBlendFactor SrcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        VkBlendFactor DstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp ColorBlendOp = VK_BLEND_OP_ADD;
        VkBlendFactor SrcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor DstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp AlphaBlendOp = VK_BLEND_OP_ADD;
        // 顶点缓冲 stride 覆盖：引擎自有顶点布局与 shader 打包布局不一致时使用
        std::optional<uint32_t> VertexStride;
    };

    // 完整着色器资源：加载 .azshader → glslc 编译多 stage SPV → SPIR-V 反射 → 生成管线。
    // 管线完全跟随 shader：顶点布局 / 描述符布局 / push constants 均由反射得到，状态来自 pipeline 段。
    // 实现前端 Shader 抽象，可通过 Azer::Shader::Create(name) 按后端分派创建。
    class VulkanShader : public Shader {
    public:
        static Ref<VulkanShader> Create(const std::string& name);

        // 相同绑定签名的 set layout 复用同一个 VkDescriptorSetLayout
        static VkDescriptorSetLayout GetSharedSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
        // 标准纹理布局（set 1：单个 combined image sampler，fragment 阶段），供 VulkanTexture 使用
        static VkDescriptorSetLayout GetStandardTextureLayout();
        // 销毁共享 set layout（应用退出 / 渲染器关闭时调用）
        static void ShutdownSharedLayouts();

        ~VulkanShader();

        inline VkPipeline GetPipeline() const { return m_Pipeline; }
        inline VkPipelineLayout GetLayout() const { return m_PipelineLayout; }
        inline VkDescriptorSetLayout GetSetLayout(uint32_t set) const
        {
            auto it = m_SetLayouts.find(set);
            AZ_ASSERT(it != m_SetLayouts.end(), "VulkanShader: missing descriptor set");
            return it == m_SetLayouts.end() ? VK_NULL_HANDLE : it->second;
        }
        inline const std::string& GetName() const override { return m_Name; }

        // ---- 反射信息（自定义绘制用）----
        struct ShaderPushRange {
            uint32_t Offset = 0;
            uint32_t Size = 0;
            VkShaderStageFlags StageFlags = 0;
        };
        struct ShaderSetBinding {
            uint32_t Binding = 0;
            VkDescriptorType Type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            uint32_t Count = 1;
            VkShaderStageFlags StageFlags = 0;
        };

        inline const std::vector<ShaderPushRange>& GetPushRanges() const { return m_PushRanges; }
        inline const std::vector<ShaderSetBinding>& GetSetBindings(uint32_t set) const
        {
            static const std::vector<ShaderSetBinding> s_Empty;
            auto it = m_Sets.find(set);
            return it == m_Sets.end() ? s_Empty : it->second;
        }

        // ---- Uniform 上传（前端 Shader::SetUniform 的实现）----
        // 所有 uniform 都走动态 uniform buffer：每帧一个大 storage + 游标，
        // SetUniform 每次调用写入新槽位并记录偏移，绘制时用当前偏移绑定（支持每绘制不同数据）。
        void SetUniform(const std::string& name, const void* data, uint32_t size) override;

        // 把当前帧的所有 uniform 描述符集绑定到命令缓冲（带动态偏移），供后端绘制时调用
        void BindFrameUniformSets(const VkCommandBuffer& cmd, uint32_t frame) const;

    private:
        explicit VulkanShader(const std::string& name);

        void LoadSource();
        void Compile();
        void Reflect();
        void BuildSetLayouts();
        void BuildPipeline();

        bool CompileStage(ShaderStage stage, const std::string& glsl, const std::string& outputPath);
        bool IsSpvUpToDate(const std::string& outputPath) const;

        std::string m_Name;
        std::string m_FilePath;   // assets/shaders/<name>.azshader
        std::string m_Directory;  // assets/shaders/<name>/
        std::string m_VertexSource;
        std::string m_FragmentSource;
        ShaderPipelineConfig m_Config;

        // 反射结果
        struct VertexAttribute {
            uint32_t Location = 0;
            VkFormat Format = VK_FORMAT_UNDEFINED;
            uint32_t Size = 0;
        };
        std::vector<VertexAttribute> m_Attributes;
        std::vector<ShaderPushRange> m_PushRanges;
        std::unordered_map<uint32_t, std::vector<ShaderSetBinding>> m_Sets; // set -> bindings

        std::vector<VkShaderModule> m_Modules;
        std::vector<VkPipelineShaderStageCreateInfo> m_Stages;
        std::unordered_map<uint32_t, VkDescriptorSetLayout> m_SetLayouts;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

        // ---- Uniform 资源（反射 + 每帧动态 storage）----
        // 与后端在途帧数一致，保证覆盖前 GPU 已完成读取
        static constexpr uint32_t FRAME_COUNT = 3;
        static constexpr VkDeviceSize DYNAMIC_STORAGE_SIZE = 2 * 1024 * 1024;

        struct UniformInfo {
            uint32_t Set = 0;
            uint32_t Binding = 0;
            uint32_t Size = 0;
        };

        struct DynamicFrame {
            Scope<VulkanUniformBuffer> storage;                       // 每帧一个大动态缓冲
            VkDeviceSize capacity = 0;                                // 当前容量（可扩容）
            VkDeviceSize cursor = 0;                                  // 写入游标
            std::unordered_map<uint32_t, Scope<VulkanDescriptorSet>> descriptorSets; // set -> 描述符集（写一次）
            std::unordered_map<uint32_t, std::map<uint32_t, uint32_t>> offsets;      // set -> binding -> 动态偏移(uint32)
        };

        std::unordered_map<std::string, UniformInfo> m_UniformInfos;           // name -> 反射信息
        std::array<DynamicFrame, FRAME_COUNT> m_DynamicFrames;
        int m_LastSetUniformFrame = -1;
        uint32_t m_UboAlignment = 256;   // minUniformBufferOffsetAlignment

        static std::unordered_map<std::string, Ref<VulkanShader>> s_Cache;
    };
}
