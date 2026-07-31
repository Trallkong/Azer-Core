//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include <string>

namespace Azer
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        inline virtual uint32_t GetWidth() const = 0;
        inline virtual uint32_t GetHeight() const = 0;
        virtual void* GetHandle() const = 0;

        // 通过后端静态上下文构造对应派生类，不需要 Renderer
        static Ref<Texture> Create(const std::string& filePath);
        static Ref<Texture> Create(void* pixels, uint32_t width, uint32_t height);

        // 禁止拷贝，只许移动
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
    protected:
        Texture() = default;
    };
}
