//
// Created by csis on 2026/9/29.
//

#pragma once
#include <cstdint>
#include <variant>

struct Color
{
    uint8_t r = 0, g = 0, b = 0, a = 255;
};

struct Vector2
{
    float x = 0.0f, y = 0.0f;
};

struct Vector2i
{
    uint32_t x = 0, y = 0;
};

struct Vector3
{
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

struct Vector3i
{
    uint32_t x = 0, y = 0, z = 0;
};

using Numeric = std::variant<int, double, float, Vector2, Vector3, Vector3i, Vector2i, Vector3>;