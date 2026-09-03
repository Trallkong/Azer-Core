//
// Created by Trallkong on 2026/8/1.
//

#pragma once

#include "Base.h"
#include "UUID.h"

namespace Azer {

    namespace Resources {

        class Resources
        {
        public:
            Resources() {
                m_UUID = generate_uuid();
            }
            virtual ~Resources() = default;

            [[nodiscard]] const std::string& GetUUID() const { return m_UUID; }

        private:
            std::string m_UUID;
        };
    }
}
