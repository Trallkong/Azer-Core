//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include "Texture.h"

#include "SDL3/SDL.h"

namespace Azer
{
    class SDL3Texture : public Texture
    {
    public:
        SDL3Texture(SDL_Texture* tex, uint32_t w, uint32_t h)
            : m_Texture(tex), m_Width(w), m_Height(h) {}
        ~SDL3Texture() override { SDL_DestroyTexture(m_Texture); }

        static Ref<Texture> Create(SDL_Renderer* renderer, const std::string& filePath);
        static Ref<Texture> Create(SDL_Renderer* renderer, void* pixels, uint32_t width, uint32_t height);
        static Ref<Texture> CreateHDR(SDL_Renderer* renderer, const std::string& filePath);

        uint32_t GetWidth() const override      { return m_Width; }
        uint32_t GetHeight() const override     { return m_Height; }
        void* GetHandle() const override        { return m_Texture; }

    private:
        SDL_Texture* m_Texture;
        uint32_t m_Width, m_Height;
    };
} // azer

