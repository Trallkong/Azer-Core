//
// Created by csis on 2026/9/30.
//

#pragma once


namespace Azer::Math
{
    inline float Lerp(const float a, const float b, const float delta)
    {
        return a + (b - a) * delta;
    }
}
