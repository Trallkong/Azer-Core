//
// Created by csis on 2026/10/5.
//

#include "Type.h"
#include "Math.h"

namespace Azer
{
    VariantValue Variant::Lerp(const VariantValue& to, const float delta) const
    {
        if (IsFloat(*m_Value) && IsFloat(to))
        {
            float v = Math::Lerp(AsFloat(*m_Value), AsFloat(to), delta);
            return v;
        }

        if (IsVector2(*m_Value) && IsVector2(to))
        {
            Vector2 v = Math::Lerp(AsVector2(*m_Value), AsVector2(to), delta);
            return v;
        }

        if (IsVector3(*m_Value) && IsVector3(to))
        {
            Vector3 v = Math::Lerp(AsVector3(*m_Value), AsVector3(to), delta);
            return v;
        }

        return -1;
    }
}
