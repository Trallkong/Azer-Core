//
// Created by Trallkong on 2026/5/29.
//

#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

namespace azer
{
    enum class VariantType
    {
        None, Float, Vec2, Vec3, Quat
    };

    class Variant
    {
    public:
        Variant();
        Variant(float v);
        Variant(const glm::vec2& v);
        Variant(const glm::vec3& v);
        Variant(const glm::quat& v);

        VariantType Type() const;
        bool IsNone() const;

        float AsFloat() const;
        glm::vec2 AsVec2() const;
        glm::vec3 AsVec3() const;
        glm::quat AsQuat() const;

        void Set(float v);
        void Set(const glm::vec2& v);
        void Set(const glm::vec3& v);
        void Set(const glm::quat& v);

        // Write this variant's value to an external memory address
        static void Write(void* dst, const Variant& value);

    private:
        void Destroy();

        VariantType m_Type = VariantType::None;
        union
        {
            float float_;
            glm::vec2 vec2_;
            glm::vec3 vec3_;
            glm::quat quat_;
        };
    };

    Variant Interpolate(const Variant& a, const Variant& b, float alpha);
}
