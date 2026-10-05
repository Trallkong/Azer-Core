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

    class Variant
    {
    public:
        explicit Variant(const VariantValue& v)
            : m_Value(v)
        {

        }

        [[nodiscard]] bool IsInt()      const { return m_Value.index() == 0; }
        [[nodiscard]] bool IsDouble()   const { return m_Value.index() == 1; }
        [[nodiscard]] bool IsFloat()    const { return m_Value.index() == 2; }
        [[nodiscard]] bool IsVector2()  const { return m_Value.index() == 3; }
        [[nodiscard]] bool IsVector3()  const { return m_Value.index() == 4; }
        [[nodiscard]] bool IsVector3i() const { return m_Value.index() == 5; }
        [[nodiscard]] bool IsVector2i() const { return m_Value.index() == 6; }

        [[nodiscard]] int       AsInt()         const { return std::get<0>(m_Value); }
        [[nodiscard]] double    AsDouble()      const { return std::get<1>(m_Value); }
        [[nodiscard]] float     AsFloat()       const { return std::get<2>(m_Value); }
        [[nodiscard]] Vector2   AsVector2()     const { return std::get<3>(m_Value); }
        [[nodiscard]] Vector3   AsVector3()     const { return std::get<4>(m_Value); }
        [[nodiscard]] Vector3i  AsVector3i()    const { return std::get<5>(m_Value); }
        [[nodiscard]] Vector2i  AsVector2i()    const { return std::get<6>(m_Value); }

        void Set(const VariantValue& v) { m_Value = v; }
        [[nodiscard]] const VariantValue& Get() const { return m_Value; }

        Variant Lerp(const Variant& to, float delta) const;

    private:
        VariantValue m_Value;
    };
}

