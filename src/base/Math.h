//
// Created by csis on 2026/9/30.
//

#pragma once


namespace Azer::Math
{
    float Lerp(float a, float b, float delta);

    Vector2 Lerp(const Vector2& a, const Vector2& b, float delta);

    Vector3 Lerp(const Vector3& a, const Vector3& b, float delta);
}
