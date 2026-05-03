//
// Created by Trallkong on 2026/5/1.
//

#ifndef AZER_DEV_GPUTEXTURE_H
#define AZER_DEV_GPUTEXTURE_H

#include "Base.h"
#include "Texture.h"

#include "SDL3/SDL.h"

namespace azer
{
    class GPUTexture : public Texture
    {
    public:
        GPUTexture(SDL_GPUDevice* device, SDL_GPUTexture* tex, SDL_GPUTextureFormat fmt, uint32_t w, uint32_t  h)
            : m_Device(device), m_Texture(tex), m_Format(fmt), m_Width(w), m_Height(h) {}
        ~GPUTexture() override { SDL_ReleaseGPUTexture(m_Device, m_Texture); }

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
        void* GetHandle() const override { return m_Texture; }

    private:
        SDL_GPUDevice* m_Device;
        SDL_GPUTexture* m_Texture;
        SDL_GPUTextureFormat m_Format;
        uint32_t m_Width, m_Height;
    };
}

#endif //AZER_DEV_GPUTEXTURE_H
