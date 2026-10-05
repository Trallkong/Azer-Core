//
// Created by csis on 2026/10/5.
//

#include "Type.h"
#include "Math.h"

namespace Azer
{
    Variant Variant::Lerp(const Variant& to, const float delta) const
    {
        if (IsFloat() && to.IsFloat())
        {
            float v = Math::Lerp(AsFloat(), to.AsFloat(), delta);
            return Variant(v);
        }

        if (IsVector2() && to.IsVector2())
        {
            Vector2 v = Math::Lerp(AsVector2(), to.AsVector2(), delta);
            return Variant(v);
        }

        if (IsVector3() && to.IsVector3())
        {
            Vector3 v = Math::Lerp(AsVector3(), to.AsVector3(), delta);
            return Variant(v);
        }

        return Variant(-1);
    }
}
