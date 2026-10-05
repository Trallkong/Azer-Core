//
// Created by csis on 2026/10/5.
//

#include "Math.h"
#include "Type.h"

namespace Azer::Math
{
    float Lerp(const float a, const float b, const float delta)
    {
        return a + (b - a) * delta;
    }

    Vector2 Lerp(const Vector2& a, const Vector2& b, const float delta)
    {
        return a + (b - a) * delta;
    }

    Vector3 Lerp(const Vector3& a, const Vector3& b, const float delta)
    {
        return a + (b - a) * delta;
    }
}
