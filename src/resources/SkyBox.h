//
// Created by Trallkong on 2026/8/1.
//
#pragma once

#include "Base.h"
#include "Resource.h"

#include "Texture.h"

#include <string>

namespace Azer {

    namespace Resources {

        class SkyBox : public Resources
        {
        public:
            explicit SkyBox(const std::string& resourcePath);
            ~SkyBox() override;

            inline const Ref<Texture>& GetTexture() const { return m_Texture; }

            float Exposure = 1.0f;
        private:
            std::string m_ResourcePath;
            Ref<Texture> m_Texture;
        };
    }
}
