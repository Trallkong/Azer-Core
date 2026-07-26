//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include <string>

namespace Azer
{
    class Renderer;

    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual void* GetHandle() const = 0;

        static Ref<Texture> Create(Renderer& renderer, const std::string& filePath);
        static Ref<Texture> Create(Renderer& renderer, void* pixels, uint32_t width, uint32_t height);
        static Ref<Texture> CreateHDR(Renderer& renderer, const std::string& filePath);

        // 禁止拷贝，只许移动
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
    protected:
        Texture() = default;
    };
}

