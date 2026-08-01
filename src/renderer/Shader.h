//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include <string>

namespace Azer
{
    // 前端着色器抽象：自定义着色器的入口。
    // Create 按当前后端分派（与 Texture::Create 同模式）。
    // Vulkan 分支使用 .azshader 资源系统（解析 → glslc 编译 → SPIR-V 反射 → 管线）。
    class Shader
    {
    public:
        virtual ~Shader() = default;

        // 通过后端静态上下文构造对应派生类，不需要 Renderer
        static Ref<Shader> Create(const std::string& name);

        virtual const std::string& GetName() const = 0;

        // 按 uniform 块名上传数据（名字由 shader 反射得到）。
        // buffer / 描述符集 / 飞行帧管理由后端内部完成，首次调用创建、之后只更新 buffer 数据。
        virtual void SetUniform(const std::string& name, const void* data, uint32_t size) = 0;
    };
}
