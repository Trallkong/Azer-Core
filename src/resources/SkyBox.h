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

        private:
            std::string m_ResourcePath;
            Ref<Texture> m_Texture;
        };
    }
}
