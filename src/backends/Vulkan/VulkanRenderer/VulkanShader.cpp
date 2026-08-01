#include "azpch.h"
#include "VulkanShader.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <sstream>

#include "VulkanRenderer.h"
#ifdef _WIN32
    #include <cstdlib>
#else
    #include <cstdlib>
    #include <sys/wait.h>
#endif

#include "VulkanContextManager.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "FileSystem.h"

#include "spirv_reflect.h"

namespace Azer {

    std::unordered_map<std::string, Ref<VulkanShader>> VulkanShader::s_Cache;
    static std::unordered_map<std::string, VkDescriptorSetLayout> s_SharedLayouts;

    static std::string Trim(const std::string& s)
    {
        size_t begin = s.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(begin, end - begin + 1);
    }

    static std::string FindGlslc()
    {
        const char* sdk = std::getenv("VULKAN_SDK");
        if (sdk != nullptr && *sdk != '\0')
        {
            std::string exe = std::string(sdk) + "/bin/glslc";
#ifdef _WIN32
            exe += ".exe";
#endif
            if (std::filesystem::exists(exe)) return exe;
        }
        return "glslc";
    }

    static uint32_t GetVkFormatSize(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R32_SFLOAT:
            case VK_FORMAT_R32_SINT:
            case VK_FORMAT_R32_UINT:         return 4;
            case VK_FORMAT_R32G32_SFLOAT:
            case VK_FORMAT_R32G32_SINT:
            case VK_FORMAT_R32G32_UINT:      return 8;
            case VK_FORMAT_R32G32B32_SFLOAT:
            case VK_FORMAT_R32G32B32_SINT:
            case VK_FORMAT_R32G32B32_UINT:   return 12;
            case VK_FORMAT_R32G32B32A32_SFLOAT:
            case VK_FORMAT_R32G32B32A32_SINT:
            case VK_FORMAT_R32G32B32A32_UINT:return 16;
            case VK_FORMAT_R16_SFLOAT:
            case VK_FORMAT_R16_SINT:
            case VK_FORMAT_R16_UINT:         return 2;
            case VK_FORMAT_R16G16_SFLOAT:
            case VK_FORMAT_R16G16_SINT:
            case VK_FORMAT_R16G16_UINT:      return 4;
            case VK_FORMAT_R16G16B16A16_SFLOAT:
            case VK_FORMAT_R16G16B16A16_SINT:
            case VK_FORMAT_R16G16B16A16_UINT:return 8;
            default:
                AZ_CORE_ERROR("GetVkFormatSize: unsupported format {}", static_cast<int>(format));
                return 0;
        }
    }

    static VkDescriptorType ToVkDescriptorType(SpvReflectDescriptorType type)
    {
        switch (type)
        {
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:                return VK_DESCRIPTOR_TYPE_SAMPLER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:          return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:          return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:   return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:   return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:         return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:         return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:       return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            default:
                AZ_CORE_ERROR("ToVkDescriptorType: unsupported descriptor type {}", static_cast<int>(type));
                return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }
    }

    Ref<VulkanShader> VulkanShader::Create(const std::string& name)
    {
        auto it = s_Cache.find(name);
        if (it != s_Cache.end()) return it->second;

        // 私有构造：必须在成员函数内 new
        Ref<VulkanShader> shader(new VulkanShader(name));
        shader->LoadSource();
        shader->Compile();
        shader->Reflect();
        shader->BuildSetLayouts();
        shader->BuildPipeline();

        s_Cache[name] = shader;
        return shader;
    }

    VulkanShader::VulkanShader(const std::string& name)
        : m_Name(name)
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(VulkanContextManager::GetContext().PhysicalDevice, &props);
        m_UboAlignment = props.limits.minUniformBufferOffsetAlignment;
        if (m_UboAlignment == 0)
        {
            m_UboAlignment = 256;
        }
    }

    VulkanShader::~VulkanShader()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        if (m_Pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(ctx.Device, m_Pipeline, nullptr);
            m_Pipeline = VK_NULL_HANDLE;
        }
        if (m_PipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(ctx.Device, m_PipelineLayout, nullptr);
            m_PipelineLayout = VK_NULL_HANDLE;
        }
        if (m_PipelineCache != VK_NULL_HANDLE)
        {
            vkDestroyPipelineCache(ctx.Device, m_PipelineCache, nullptr);
            m_PipelineCache = VK_NULL_HANDLE;
        }

        for (VkShaderModule module : m_Modules)
        {
            if (module != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(ctx.Device, module, nullptr);
            }
        }
        m_Modules.clear();

        // m_SetLayouts 是共享 layout，统一由 ShutdownSharedLayouts 销毁
    }

    // ==================== 解析 ====================

    void VulkanShader::LoadSource()
    {
        m_FilePath = std::filesystem::path(
            FileSystem::ResolvePath("./assets/shaders/" + m_Name + ".azshader")
        ).lexically_normal().string();

        std::string text = FileSystem::ReadText(m_FilePath);
        if (text.empty())
        {
            AZ_CORE_ERROR("VulkanShader: failed to read shader file: {0}", m_FilePath);
            return;
        }

        enum class Section { None, Vertex, Fragment, Pipeline };

        Section section = Section::None;
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line))
        {
            std::string trimmed = Trim(line);
            if (trimmed.empty())
            {
                continue;
            }

            if (trimmed[0] == '@')
            {
                if (trimmed == "@vertex")
                {
                    section = Section::Vertex;
                    continue;
                }
                if (trimmed == "@fragment")
                {
                    section = Section::Fragment;
                    continue;
                }
                if (trimmed == "@pipeline")
                {
                    section = Section::Pipeline;
                    continue;
                }
                if (trimmed.rfind("@name", 0) == 0)
                {
                    std::string nm = Trim(trimmed.substr(5));
                    if (!nm.empty()) m_Name = nm;
                    section = Section::None;
                    continue;
                }
                section = Section::None;
                continue;
            }

            if (section == Section::Pipeline)
            {
                if (trimmed[0] == '#') continue;                        // 注释行
                size_t colon = line.find(':');
                if (colon == std::string::npos) continue;
                std::string key = Trim(line.substr(0, colon));
                std::string value = Trim(line.substr(colon + 1));

                if (key == "topology")
                {
                    if (value == "triangle_list")      m_Config.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    else if (value == "triangle_strip") m_Config.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
                    else if (value == "line_list")       m_Config.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    else if (value == "point_list")      m_Config.Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                }
                else if (key == "cull_mode")
                {
                    if (value == "none")                m_Config.CullMode = VK_CULL_MODE_NONE;
                    else if (value == "front")          m_Config.CullMode = VK_CULL_MODE_FRONT_BIT;
                    else if (value == "back")           m_Config.CullMode = VK_CULL_MODE_BACK_BIT;
                }
                else if (key == "front_face")
                {
                    if (value == "cw")                  m_Config.FrontFace = VK_FRONT_FACE_CLOCKWISE;
                    else if (value == "ccw")            m_Config.FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                }
                else if (key == "depth_test")
                {
                    m_Config.DepthTestEnable = (value == "true" || value == "1");
                }
                else if (key == "depth_write")
                {
                    m_Config.DepthWriteEnable = (value == "true" || value == "1");
                }
                else if (key == "depth_clamp")
                {
                    m_Config.DepthClampEnable = (value == "true" || value == "1");
                }
                else if (key == "blend")
                {
                    if (value == "none")
                    {
                        m_Config.BlendEnable = false;
                    }
                    else if (value == "additive")
                    {
                        m_Config.BlendEnable = true;
                        m_Config.SrcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                        m_Config.DstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                        m_Config.ColorBlendOp = VK_BLEND_OP_ADD;
                        m_Config.SrcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        m_Config.DstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        m_Config.AlphaBlendOp = VK_BLEND_OP_ADD;
                    }
                    // alpha（默认）保持默认值
                }
                else if (key == "vertex_stride")
                {
                    m_Config.VertexStride = static_cast<uint32_t>(std::strtoul(value.c_str(), nullptr, 10));
                }
            }
            else if (section == Section::Vertex)
            {
                m_VertexSource += line + "\n";
            }
            else if (section == Section::Fragment)
            {
                m_FragmentSource += line + "\n";
            }
        }

        m_Directory = (std::filesystem::path(m_FilePath).parent_path() / m_Name).string();
    }

    // ==================== 编译 ====================

    bool VulkanShader::IsSpvUpToDate(const std::string& outputPath) const
    {
        namespace fs = std::filesystem;
        if (!fs::exists(outputPath)) return false;
        if (!fs::exists(m_FilePath)) return false;

        std::error_code ec;
        auto outTime = fs::last_write_time(outputPath, ec);
        auto srcTime = fs::last_write_time(m_FilePath, ec);
        if (ec) return false;

        // 以 .azshader 源文件时间戳为准：.spv 更新即视为有效，
        // 避免因重写 .glsl 中间文件导致每次都重新编译
        return outTime >= srcTime;
    }

    bool VulkanShader::CompileStage(ShaderStage stage, const std::string& glsl, const std::string& outputPath)
    {
        const std::string inputPath = outputPath.substr(0, outputPath.size() - 4) + ".glsl";

        // 先判断缓存再写 .glsl：否则每次重写都会让中间文件比 .spv 新，导致每次都重编译
        if (IsSpvUpToDate(outputPath))
        {
            AZ_CORE_DEBUG("VulkanShader: '{0}' up to date, skipping compile", m_Name);
            return true;
        }

        if (!std::filesystem::exists(m_Directory))
        {
            std::filesystem::create_directories(m_Directory);
        }
        FileSystem::WriteText(inputPath, glsl);

        const char* stageFlag = (stage == ShaderStage::VERTEX) ? "vertex" : "fragment";
        std::string glslc = FindGlslc();
        std::string cmd = "\"" + glslc + "\" -fshader-stage=" + stageFlag
                        + " \"" + inputPath + "\" -o \"" + outputPath + "\" 2>&1";

        AZ_CORE_INFO("VulkanShader: compiling '{0}' ({1} stage)", m_Name, stageFlag);

#ifdef _WIN32
        // _popen 通过 cmd /c 执行：命令以引号开头时，cmd 会剥掉首引号导致路径损坏，
        // 整体再包一层引号即可规避。
        std::string wrapped = "\"" + cmd + "\"";
        FILE* pipe = _popen(wrapped.c_str(), "r");
#else
        FILE* pipe = popen(cmd.c_str(), "r");
#endif
        std::string output;
        if (pipe != nullptr)
        {
            char buffer[512];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                output += buffer;
            }
#ifdef _WIN32
            int rc = _pclose(pipe);
#else
            int rc = pclose(pipe);
            if (rc != -1) rc = WEXITSTATUS(rc);
#endif
            if (rc != 0)
            {
                AZ_CORE_ERROR("VulkanShader: glslc failed for '{0}':\n{1}", m_Name, output);
                return false;
            }
        }
        else
        {
            AZ_CORE_ERROR("VulkanShader: failed to run glslc ('{0}'). Make sure the Vulkan SDK is installed and VULKAN_SDK is set.", glslc);
            return false;
        }

        return true;
    }

    void VulkanShader::Compile()
    {
        if (!m_VertexSource.empty())
        {
            CompileStage(ShaderStage::VERTEX, m_VertexSource,
                (std::filesystem::path(m_Directory) / (m_Name + ".vert.spv")).string());
        }
        if (!m_FragmentSource.empty())
        {
            CompileStage(ShaderStage::FRAGMENT, m_FragmentSource,
                (std::filesystem::path(m_Directory) / (m_Name + ".frag.spv")).string());
        }
    }

    // ==================== 反射 ====================

    void VulkanShader::Reflect()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        auto reflectStage = [&](ShaderStage stage, VkShaderStageFlags stageFlags, const std::string& spvPath)
        {
            std::vector<uint8_t> bytes = FileSystem::ReadBytes(spvPath);
            if (bytes.empty())
            {
                AZ_CORE_ERROR("VulkanShader: failed to read SPV: {0}", spvPath);
                return;
            }

            VkShaderModuleCreateInfo moduleInfo{};
            moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleInfo.codeSize = bytes.size();
            moduleInfo.pCode = reinterpret_cast<const uint32_t*>(bytes.data());
            VkShaderModule module = VK_NULL_HANDLE;
            VkResult result = vkCreateShaderModule(ctx.Device, &moduleInfo, nullptr, &module);
            AZ_ASSERT(result == VK_SUCCESS, "Failed to create shader module");
            m_Modules.push_back(module);

            VkPipelineShaderStageCreateInfo stageInfo{};
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = static_cast<VkShaderStageFlagBits>(stageFlags);
            stageInfo.module = module;
            stageInfo.pName = "main";
            m_Stages.push_back(stageInfo);

            SpvReflectShaderModule refModule{};
            SpvReflectResult refResult = spvReflectCreateShaderModule(bytes.size(), bytes.data(), &refModule);
            if (refResult != SPV_REFLECT_RESULT_SUCCESS)
            {
                AZ_CORE_ERROR("VulkanShader: SPIRV-Reflect failed on {0}", spvPath);
                return;
            }

            // 顶点输入属性（仅 vertex 阶段）
            if (stage == ShaderStage::VERTEX)
            {
                uint32_t count = 0;
                spvReflectEnumerateInputVariables(&refModule, &count, nullptr);
                std::vector<SpvReflectInterfaceVariable*> vars(count);
                if (count > 0)
                {
                    spvReflectEnumerateInputVariables(&refModule, &count, vars.data());
                }
                for (uint32_t i = 0; i < count; ++i)
                {
                    const SpvReflectInterfaceVariable* var = vars[i];
                    if (var == nullptr) continue;
                    if (var->built_in != -1) continue;                      // 跳过内置变量

                    VertexAttribute attr;
                    attr.Location = var->location;
                    attr.Format = static_cast<VkFormat>(var->format);
                    attr.Size = GetVkFormatSize(attr.Format);
                    m_Attributes.push_back(attr);
                }
            }

            // 描述符集
            uint32_t setCount = 0;
            spvReflectEnumerateDescriptorSets(&refModule, &setCount, nullptr);
            std::vector<SpvReflectDescriptorSet*> sets(setCount);
            if (setCount > 0)
            {
                spvReflectEnumerateDescriptorSets(&refModule, &setCount, sets.data());
            }
            for (uint32_t i = 0; i < setCount; ++i)
            {
                const SpvReflectDescriptorSet* rs = sets[i];
                for (uint32_t b = 0; b < rs->binding_count; ++b)
                {
                    const SpvReflectDescriptorBinding* rb = rs->bindings[b];
                    if (rb == nullptr) continue;

                    // uniform 块：记录变量名 -> set/binding/块大小，供 SetUniform 按名上传
                    if (rb->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                        && rb->name != nullptr)
                    {
                        UniformInfo info;
                        info.Set = rs->set;
                        info.Binding = rb->binding;
                        info.Size = rb->block.size;
                        m_UniformInfos[rb->name] = info;
                    }

                    ShaderSetBinding sb;
                    sb.Binding = rb->binding;
                    sb.Type = ToVkDescriptorType(rb->descriptor_type);
                    sb.Count = rb->count == 0 ? 1 : rb->count;
                    sb.StageFlags = stageFlags;

                    auto& bindings = m_Sets[rs->set];
                    auto it = std::find_if(bindings.begin(), bindings.end(),
                        [&](const ShaderSetBinding& x) { return x.Binding == sb.Binding; });
                    if (it != bindings.end())
                    {
                        it->StageFlags |= stageFlags;
                    }
                    else
                    {
                        bindings.push_back(sb);
                    }
                }
            }

            // push constants
            uint32_t pushCount = 0;
            spvReflectEnumeratePushConstantBlocks(&refModule, &pushCount, nullptr);
            std::vector<SpvReflectBlockVariable*> blocks(pushCount);
            if (pushCount > 0)
            {
                spvReflectEnumeratePushConstantBlocks(&refModule, &pushCount, blocks.data());
            }
            for (uint32_t i = 0; i < pushCount; ++i)
            {
                const SpvReflectBlockVariable* block = blocks[i];
                if (block == nullptr) continue;

                ShaderPushRange pr;
                pr.Offset = block->offset;
                pr.Size = block->size;
                pr.StageFlags = stageFlags;

                auto it = std::find_if(m_PushRanges.begin(), m_PushRanges.end(),
                    [&](const ShaderPushRange& x) { return x.Offset == pr.Offset && x.Size == pr.Size; });
                if (it != m_PushRanges.end())
                {
                    it->StageFlags |= stageFlags;
                }
                else
                {
                    m_PushRanges.push_back(pr);
                }
            }

            spvReflectDestroyShaderModule(&refModule);
        };

        if (!m_VertexSource.empty())
        {
            reflectStage(ShaderStage::VERTEX, VK_SHADER_STAGE_VERTEX_BIT,
                (std::filesystem::path(m_Directory) / (m_Name + ".vert.spv")).string());
        }
        if (!m_FragmentSource.empty())
        {
            reflectStage(ShaderStage::FRAGMENT, VK_SHADER_STAGE_FRAGMENT_BIT,
                (std::filesystem::path(m_Directory) / (m_Name + ".frag.spv")).string());
        }

        std::sort(m_Attributes.begin(), m_Attributes.end(),
            [](const VertexAttribute& a, const VertexAttribute& b) { return a.Location < b.Location; });
    }

    void VulkanShader::BuildSetLayouts()
    {
        for (auto& [set, bindings] : m_Sets)
        {
            std::sort(bindings.begin(), bindings.end(),
                [](const ShaderSetBinding& a, const ShaderSetBinding& b) { return a.Binding < b.Binding; });

            std::vector<VkDescriptorSetLayoutBinding> vkBindings;
            vkBindings.reserve(bindings.size());
            for (const auto& b : bindings)
            {
                VkDescriptorSetLayoutBinding vk{};
                vk.binding = b.Binding;
                // 统一按动态 uniform buffer 处理（每绘制一个 offset）
                vk.descriptorType = (b.Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                                  ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                                  : b.Type;
                vk.descriptorCount = b.Count;
                vk.stageFlags = b.StageFlags;
                vkBindings.push_back(vk);
            }

            m_SetLayouts[set] = GetSharedSetLayout(vkBindings);
        }
    }

    // ==================== Uniform 上传 ====================

    void VulkanShader::SetUniform(const std::string& name, const void* data, uint32_t size)
    {
        auto it = m_UniformInfos.find(name);
        if (it == m_UniformInfos.end())
        {
            AZ_CORE_WARN("VulkanShader: shader '{0}' has no uniform block named '{1}'", m_Name, name);
            return;
        }
        const UniformInfo& info = it->second;

        auto* renderer = VulkanRenderer::Get();
        uint32_t frame = renderer != nullptr ? renderer->GetCurrentFrameIndex() % FRAME_COUNT : 0;

        DynamicFrame& dyn = m_DynamicFrames[frame];

        // 帧切换 = 新一轮绘制：上一帧用量过半则在此时安全扩容（本帧尚未写入、GPU 未引用旧 buffer）
        if (m_LastSetUniformFrame != static_cast<int>(frame))
        {
            if (dyn.storage != nullptr && dyn.cursor >= dyn.capacity / 2)
            {
                VkDeviceSize newCapacity = std::max<VkDeviceSize>(dyn.capacity * 2, DYNAMIC_STORAGE_SIZE);
                dyn.capacity = newCapacity;
                dyn.storage = CreateScope<VulkanUniformBuffer>(newCapacity);
                dyn.descriptorSets.clear();   // 描述符集引用旧 buffer，需用新 buffer 重建
                dyn.offsets.clear();
            }
            dyn.cursor = 0;
            m_LastSetUniformFrame = static_cast<int>(frame);
        }

        if (!dyn.storage)
        {
            dyn.capacity = DYNAMIC_STORAGE_SIZE;
            dyn.storage = CreateScope<VulkanUniformBuffer>(dyn.capacity);
        }

        // 容量保护：越界则跳过（不应发生，扩容逻辑已按帧推进）
        if (dyn.cursor + static_cast<VkDeviceSize>(size) > dyn.capacity)
        {
            AZ_CORE_ERROR("VulkanShader: dynamic uniform storage exceeded capacity for shader '{0}'", m_Name);
            return;
        }

        // 首次访问该 set：创建描述符集，把所有 uniform 绑定指向 storage（offset 0，range = 各自大小）
        if (dyn.descriptorSets.find(info.Set) == dyn.descriptorSets.end())
        {
            if (!dyn.storage)
            {
                dyn.storage = CreateScope<VulkanUniformBuffer>(DYNAMIC_STORAGE_SIZE);
            }

            auto descriptorSet = CreateScope<VulkanDescriptorSet>(this, info.Set);
            for (const auto& [n, u] : m_UniformInfos)
            {
                (void)n;
                if (u.Set == info.Set)
                {
                    descriptorSet->SetBuffer(u.Binding, dyn.storage->GetBuffer(), u.Size);
                }
            }
            descriptorSet->Update();
            dyn.descriptorSets[info.Set] = std::move(descriptorSet);
        }

        // 写入当前游标并记录偏移
        dyn.storage->SetData(data, size, dyn.cursor);
        dyn.offsets[info.Set][info.Binding] = static_cast<uint32_t>(dyn.cursor);

        // 游标前进（对齐到 minUniformBufferOffsetAlignment）
        VkDeviceSize aligned = (static_cast<VkDeviceSize>(size) + m_UboAlignment - 1)
                             & ~(static_cast<VkDeviceSize>(m_UboAlignment) - 1);
        dyn.cursor += aligned;

        if (dyn.cursor > DYNAMIC_STORAGE_SIZE)
        {
            AZ_CORE_WARN("VulkanShader: dynamic uniform storage overflow for shader '{0}'", m_Name);
        }
    }

    void VulkanShader::BindFrameUniformSets(const VkCommandBuffer& cmd, uint32_t frame) const
    {
        frame %= FRAME_COUNT;
        const DynamicFrame& dyn = m_DynamicFrames[frame];

        for (const auto& [set, descriptorSet] : dyn.descriptorSets)
        {
            // 动态偏移按 binding 升序收集（对应 set layout 中的动态绑定顺序）
            std::vector<uint32_t> offsets;
            auto it = dyn.offsets.find(set);
            if (it != dyn.offsets.end())
            {
                for (const auto& [binding, offset] : it->second)
                {
                    (void)binding;
                    offsets.push_back(offset);
                }
            }

            VkDescriptorSet handle = descriptorSet->GetHandle();
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,
                set, 1, &handle,
                static_cast<uint32_t>(offsets.size()),
                offsets.empty() ? nullptr : offsets.data());
        }
    }

    // ==================== 管线 ====================

    void VulkanShader::BuildPipeline()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        // --- 顶点输入（反射 + stride 覆盖）---
        std::vector<VkVertexInputAttributeDescription> attributeDescs;
        attributeDescs.reserve(m_Attributes.size());
        uint32_t packedStride = 0;
        for (const auto& attr : m_Attributes)
        {
            VkVertexInputAttributeDescription desc{};
            desc.location = attr.Location;
            desc.binding = 0;
            desc.format = attr.Format;
            desc.offset = packedStride;
            attributeDescs.push_back(desc);
            packedStride += attr.Size;
        }

        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = m_Config.VertexStride.value_or(packedStride);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = attributeDescs.empty() ? 0 : 1;
        vertexInputInfo.pVertexBindingDescriptions = attributeDescs.empty() ? nullptr : &bindingDesc;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescs.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescs.empty() ? nullptr : attributeDescs.data();

        // --- 输入装配 ---
        VkPipelineInputAssemblyStateCreateInfo inputInfo{};
        inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputInfo.topology = m_Config.Topology;

        // --- 动态 viewport / scissor ---
        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStates.data();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.scissorCount = 1;

        // --- 光栅化 ---
        VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
        rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = m_Config.DepthClampEnable;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizerInfo.cullMode = m_Config.CullMode;
        rasterizerInfo.frontFace = m_Config.FrontFace;
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        rasterizerInfo.lineWidth = 1.0f;

        // --- 多重采样 ---
        VkPipelineMultisampleStateCreateInfo multiSampleInfo{};
        multiSampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multiSampleInfo.sampleShadingEnable = VK_FALSE;

        // --- 深度模板 ---
        // 管线声明了 depthAttachmentFormat，就必须始终提供 pDepthStencilState（见 VUID-renderPass-09033），
        // 是否真正启用深度测试由配置决定
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = m_Config.DepthTestEnable;
        depthStencilInfo.depthWriteEnable = m_Config.DepthWriteEnable;
        depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

        // --- 混合 ---
        VkPipelineColorBlendAttachmentState colorAttachment{};
        colorAttachment.blendEnable = m_Config.BlendEnable;
        colorAttachment.srcColorBlendFactor = m_Config.SrcColorBlendFactor;
        colorAttachment.dstColorBlendFactor = m_Config.DstColorBlendFactor;
        colorAttachment.colorBlendOp = m_Config.ColorBlendOp;
        colorAttachment.srcAlphaBlendFactor = m_Config.SrcAlphaBlendFactor;
        colorAttachment.dstAlphaBlendFactor = m_Config.DstAlphaBlendFactor;
        colorAttachment.alphaBlendOp = m_Config.AlphaBlendOp;
        colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                      | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{};
        colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments = &colorAttachment;
        colorBlendStateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;

        // --- 描述符集 layout（0..maxSet 排列，缺位补空 layout）---
        uint32_t maxSet = 0;
        for (const auto& [set, unused] : m_Sets)
        {
            (void)unused;
            maxSet = std::max(maxSet, set);
        }

        std::vector<VkDescriptorSetLayout> setLayouts(maxSet + 1, VK_NULL_HANDLE);
        for (const auto& [set, layout] : m_SetLayouts)
        {
            setLayouts[set] = layout;
        }
        // 空缺的 set 用空描述符布局补齐
        for (uint32_t s = 0; s <= maxSet; ++s)
        {
            if (setLayouts[s] == VK_NULL_HANDLE)
            {
                setLayouts[s] = GetSharedSetLayout({});
            }
        }

        std::vector<VkPushConstantRange> pushRanges;
        pushRanges.reserve(m_PushRanges.size());
        for (const auto& pr : m_PushRanges)
        {
            pushRanges.push_back({ pr.StageFlags, pr.Offset, pr.Size });
        }

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutInfo.pSetLayouts = setLayouts.data();
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushRanges.size());
        layoutInfo.pPushConstantRanges = pushRanges.empty() ? nullptr : pushRanges.data();

        VkResult result = vkCreatePipelineLayout(ctx.Device, &layoutInfo, nullptr, &m_PipelineLayout);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout for shader");

        // --- 动态渲染 ---
        VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
        pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRenderingInfo.colorAttachmentCount = 1;
        pipelineRenderingInfo.pColorAttachmentFormats = &ctx.Swapchain->GetFormat();
        // 渲染 pass 总是带深度附件，管线必须声明一致的深度格式（见 VUID-vkCmdDrawIndexed-dynamicRenderingUnusedAttachments-08914）
        pipelineRenderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pInputAssemblyState = &inputInfo;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.pColorBlendState = &colorBlendStateInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pMultisampleState = &multiSampleInfo;
        pipelineInfo.pRasterizationState = &rasterizerInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(m_Stages.size());
        pipelineInfo.pStages = m_Stages.data();
        pipelineInfo.pNext = &pipelineRenderingInfo;
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        VkPipelineCacheCreateInfo cacheInfo{};
        cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        vkCreatePipelineCache(ctx.Device, &cacheInfo, nullptr, &m_PipelineCache);

        result = vkCreateGraphicsPipelines(ctx.Device, m_PipelineCache, 1, &pipelineInfo, nullptr, &m_Pipeline);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline for shader");
    }

    // ==================== 共享 set layout ====================

    VkDescriptorSetLayout VulkanShader::GetSharedSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    {
        std::string key;
        for (const auto& b : bindings)
        {
            key += std::to_string(b.binding) + ":"
                 + std::to_string(b.descriptorType) + ":"
                 + std::to_string(b.descriptorCount) + ":"
                 + std::to_string(b.stageFlags) + ";";
        }

        auto it = s_SharedLayouts.find(key);
        if (it != s_SharedLayouts.end()) return it->second;

        const VulkanContext& ctx = VulkanContextManager::GetContext();
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(bindings.size());
        info.pBindings = bindings.empty() ? nullptr : bindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VkResult result = vkCreateDescriptorSetLayout(ctx.Device, &info, nullptr, &layout);
        AZ_ASSERT(result == VK_SUCCESS, "Failed to create shared descriptor set layout");

        s_SharedLayouts[key] = layout;
        return layout;
    }

    VkDescriptorSetLayout VulkanShader::GetStandardTextureLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        return GetSharedSetLayout(bindings);
    }

    void VulkanShader::ShutdownSharedLayouts()
    {
        const VulkanContext& ctx = VulkanContextManager::GetContext();

        for (const auto& [key, layout] : s_SharedLayouts)
        {
            (void)key;
            if (layout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(ctx.Device, layout, nullptr);
            }
        }
        s_SharedLayouts.clear();

        s_Cache.clear();
    }
}
