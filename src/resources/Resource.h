//
// Created by Trallkong on 2026/8/1.
//

#pragma once

#include "Base.h"
#include "boost/uuid.hpp"

namespace Azer {

    namespace Resources {

        class Resources
        {
        public:
            Resources() {
                boost::uuids::random_generator gen;
                auto uuid = gen();
            }
            virtual ~Resources() = default;

        private:
            uint32_t m_UUID;
        };
    }
}
