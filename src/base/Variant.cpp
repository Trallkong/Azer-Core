//
// Created by Trallkong on 2026/5/29.
//

#include "azpch.h"
#include "Variant.h"

namespace Azer
{
    Variant::Variant() {}

    Variant::Variant(float v)
        : m_Type(VariantType::Float)
    {
        new (&float_) float(v);
    }

    Variant::Variant(const glm::vec2& v)
        : m_Type(VariantType::Vec2)
    {
        new (&vec2_) glm::vec2(v);
    }

    Variant::Variant(const glm::vec3& v)
        : m_Type(VariantType::Vec3)
    {
        new (&vec3_) glm::vec3(v);
    }

    Variant::Variant(const glm::quat& v)
        : m_Type(VariantType::Quat)
    {
        new (&quat_) glm::quat(v);
    }

    VariantType Variant::Type() const
    {
        return m_Type;
    }

    bool Variant::IsNone() const
    {
        return m_Type == VariantType::None;
    }

    float Variant::AsFloat() const
    {
        assert(m_Type == VariantType::Float);
        return float_;
    }

    glm::vec2 Variant::AsVec2() const
    {
        assert(m_Type == VariantType::Vec2);
        return vec2_;
    }

    glm::vec3 Variant::AsVec3() const
    {
        assert(m_Type == VariantType::Vec3);
        return vec3_;
    }

    glm::quat Variant::AsQuat() const
    {
        assert(m_Type == VariantType::Quat);
        return quat_;
    }

    void Variant::Set(float v)
    {
        Destroy();
        m_Type = VariantType::Float;
        new (&float_) float(v);
    }

    void Variant::Set(const glm::vec2& v)
    {
        Destroy();
        m_Type = VariantType::Vec2;
        new (&vec2_) glm::vec2(v);
    }

    void Variant::Set(const glm::vec3& v)
    {
        Destroy();
        m_Type = VariantType::Vec3;
        new (&vec3_) glm::vec3(v);
    }

    void Variant::Set(const glm::quat& v)
    {
        Destroy();
        m_Type = VariantType::Quat;
        new (&quat_) glm::quat(v);
    }

    void Variant::Destroy()
    {
        // glm types are trivially destructible, no explicit destructor call needed
        m_Type = VariantType::None;
    }

    void Variant::Write(void* dst, const Variant& value)
    {
        assert(dst);
        switch (value.Type())
        {
            case VariantType::Float:
                *static_cast<float*>(dst) = value.AsFloat();
                break;
            case VariantType::Vec2:
                *static_cast<glm::vec2*>(dst) = value.AsVec2();
                break;
            case VariantType::Vec3:
                *static_cast<glm::vec3*>(dst) = value.AsVec3();
                break;
            case VariantType::Quat:
                *static_cast<glm::quat*>(dst) = value.AsQuat();
                break;
            default:
                break;
        }
    }

    Variant Interpolate(const Variant& a, const Variant& b, float alpha)
    {
        assert(a.Type() == b.Type());

        switch (a.Type())
        {
            case VariantType::Float:
                return Variant(glm::mix(a.AsFloat(), b.AsFloat(), alpha));
            case VariantType::Vec2:
                return Variant(glm::mix(a.AsVec2(), b.AsVec2(), alpha));
            case VariantType::Vec3:
                return Variant(glm::mix(a.AsVec3(), b.AsVec3(), alpha));
            case VariantType::Quat:
                return Variant(glm::slerp(a.AsQuat(), b.AsQuat(), alpha));
            default:
                return Variant();
        }
    }

} // azer
