//
// Created by Trallkong on 2026/8/1.
//

#include "azpch.h"
#include "SkyBox.h"

namespace Azer {

    namespace Resources {

        SkyBox::SkyBox(const std::string& resourcePath)
            : m_ResourcePath(resourcePath)
        {
            m_Texture = Texture::Create(m_ResourcePath, true);
        }

        SkyBox::~SkyBox() {
            m_Texture.reset();
        }
    }
}