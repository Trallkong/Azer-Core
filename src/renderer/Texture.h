//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include <string>

namespace Azer
{
    class Shader;

    class Texture
    {
    public:
        virtual ~Texture() = default;
        inline virtual uint32_t GetWidth() const = 0;
        inline virtual uint32_t GetHeight() const = 0;
        virtual void* GetHandle() const = 0;

        // 绑定纹理到某个 set（当前约定 set 1 为纹理集），用该 shader 的布局直接录制到当前命令缓冲。
        // 后端实现；不支持的后端用默认空实现。
        virtual void Bind(uint32_t binding, const Ref<Shader>& shader) {}

        // 通过后端静态上下文构造对应派生类，不需要 Renderer
        static Ref<Texture> Create(const std::string& filePath, bool isHDR = false);
        static Ref<Texture> Create(void* pixels, uint32_t width, uint32_t height);

        // 禁止拷贝，只许移动
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
    protected:
        Texture() = default;
    };
}
