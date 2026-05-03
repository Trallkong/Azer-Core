//
// Created by Trallkong on 2026/4/30.
//

#ifndef AZER_RENDERER2D_H
#define AZER_RENDERER2D_H

#include "Base.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "SDL3/SDL.h"
#include <string>

namespace azer
{
    class Renderer2D
    {
    public:
        static void DrawQuad(float x, float y, float w, float h);
        static void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color);
        static Scope<Texture> CreateTexture(const std::string& filePath);
        static void DrawTexture(Texture* texture, const SDL_FRect& src, const SDL_FRect& dst, float angle = 0.0f);
    };
}



#endif //AZER_RENDERER2D_H
