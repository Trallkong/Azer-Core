//
// Created by Trallkong on 2026/5/30.
//

#pragma once

#include "Base.h"
#include "Variant.h"

#include <functional>

namespace azer
{
    using SetterFn = std::function<void(const Variant&)>;

    struct PropertyAccessor
    {
        void* target        = nullptr;
        VariantType type    = VariantType::None;
        SetterFn setter     = nullptr;

        void Apply(const Variant& value) const
        {
            if (setter)
            {
                setter(value);
            }
            else if (target)
            {
                Variant::Write(target, value);
            }
        }
    };
} // azer
