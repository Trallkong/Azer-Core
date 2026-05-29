//
// Created by Trallkong on 2026/5/30.
//

#pragma once

#include "Base.h"
#include "reflection/PropertyAccessor.h"

#include <string>
#include <vector>

namespace azer
{
    struct KeyFrame
    {
        Variant value;
        float time = 0.0f;
    };

    struct AnimationChannel
    {
        PropertyAccessor propertyAccessor;
        std::vector<KeyFrame> keyFrames;
    };

    struct Animation
    {
        std::string name;
        std::vector<AnimationChannel> channels;
    };
} // azer
