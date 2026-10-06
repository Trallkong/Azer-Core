//
// Created by csis on 2026/9/29.
//

#pragma once
#include <cstdint>
#include <variant>

namespace Azer
{
    struct Color
    {
        uint8_t r = 0, g = 0, b = 0, a = 255;
    };

    struct Vector2
    {
        float x = 0.0f, y = 0.0f;

        Vector2 operator+(const Vector2& rhs) const { return Vector2{.x = x + rhs.x, .y = y + rhs.y}; }
        Vector2 operator-(const Vector2& rhs) const { return Vector2{.x = x - rhs.x, .y = y - rhs.y}; }
        Vector2 operator*(const float rhs) const { return Vector2{.x = x * rhs, .y = y * rhs}; }
    };

    struct Vector2i
    {
        uint32_t x = 0, y = 0;

        Vector2i operator+(const Vector2i& rhs) const { return Vector2i{.x = x + rhs.x, .y = y + rhs.y}; }
        Vector2i operator-(const Vector2i& rhs) const { return Vector2i{.x = x - rhs.x, .y = y - rhs.y}; }
        Vector2i operator*(const int rhs) const { return Vector2i{.x = x * rhs, .y = y * rhs}; }
    };

    struct Vector3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        Vector3 operator+(const Vector3& rhs) const { return Vector3{.x = x + rhs.x, .y = y + rhs.y, .z = z}; }
        Vector3 operator-(const Vector3& rhs) const { return Vector3{.x = x - rhs.x, .y = y - rhs.y, .z = z}; }
        Vector3 operator*(const float rhs) const { return Vector3{.x = x * rhs, .y = y * rhs, .z = z * rhs}; }
    };

    struct Vector3i
    {
        uint32_t x = 0, y = 0, z = 0;

        Vector3i operator+(const Vector3i& rhs) const { return Vector3i{.x = x + rhs.x, .y = y + rhs.y, .z = z + rhs.z}; }
        Vector3i operator-(const Vector3i& rhs) const { return Vector3i{.x = x - rhs.x, .y = y - rhs.y, .z = z - rhs.z}; }
        Vector3i operator*(const int rhs) const { return Vector3i{.x = x * rhs, .y = y * rhs, .z = z * rhs}; }
    };

    using VariantValue = std::variant<int, double, float, Vector2, Vector3, Vector3i, Vector2i>;

    // 将VariantValue类型交给Variant容器托管，指针依赖，可能有内存安全问题，使用时注意。
    class Variant
    {
    public:
        explicit Variant(VariantValue* v)
            : m_Value(v)
        {

        }

        static bool IsInt(const VariantValue& value)        { return value.index() == 0; }
        static bool IsDouble(const VariantValue& value)     { return value.index() == 1; }
        static bool IsFloat(const VariantValue& value)      { return value.index() == 2; }
        static bool IsVector2(const VariantValue& value)    { return value.index() == 3; }
        static bool IsVector3(const VariantValue& value)    { return value.index() == 4; }
        static bool IsVector3i(const VariantValue& value)   { return value.index() == 5; }
        static bool IsVector2i(const VariantValue& value)   { return value.index() == 6; }

        static int       AsInt(const VariantValue& value)          { return std::get<0>(value); }
        static double    AsDouble(const VariantValue& value)       { return std::get<1>(value); }
        static float     AsFloat(const VariantValue& value)        { return std::get<2>(value); }
        static Vector2   AsVector2(const VariantValue& value)      { return std::get<3>(value); }
        static Vector3   AsVector3(const VariantValue& value)      { return std::get<4>(value); }
        static Vector3i  AsVector3i(const VariantValue& value)     { return std::get<5>(value); }
        static Vector2i  AsVector2i(const VariantValue& value)     { return std::get<6>(value); }

        void Set(const VariantValue& v) const { *m_Value = v; }
        [[nodiscard]] const VariantValue& Get() const { return *m_Value; }

        [[nodiscard]] VariantValue Lerp(const VariantValue& to, float delta) const;

    private:
        VariantValue* m_Value;
    };
}

