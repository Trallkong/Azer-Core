//
// Created by Trallkong on 2026/5/1.
//

#ifndef AZER_DEV_TEXTURE_H
#define AZER_DEV_TEXTURE_H

#include "Base.h"
#include <string>

namespace azer
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual void* GetHandle() const = 0;

        // 禁止拷贝，只许移动
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
    protected:
        Texture() = default;
    };
}

#endif //AZER_DEV_TEXTURE_H
