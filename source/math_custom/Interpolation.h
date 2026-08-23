#pragma once
#include "Vector3.h"
#include "Quat.h"
#include <cmath>
#include <algorithm>

namespace Math
{

    inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
    {
        return Vector3(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

    inline float LerpFloat(const float& a, const float& b, float t)
    {
        return a + (b - a) * t;
    }

    inline Quat Nlerp(const Quat& a, const Quat& b, float t)
    {
        Quat bAdjusted = b;
        if (a.dot(b) < 0.0f)
        {
            bAdjusted = Quat(-b.x, -b.y, -b.z, -b.w);
        }

        Quat result(
            a.x + (bAdjusted.x - a.x) * t,
            a.y + (bAdjusted.y - a.y) * t,
            a.z + (bAdjusted.z - a.z) * t,
            a.w + (bAdjusted.w - a.w) * t
        );

        return result.normalized();
    }

    inline Quat Slerp(const Quat& a, const Quat& b, float t)
    {
        float cosHalfTheta = a.dot(b);

        Quat bAdjusted = b;
        if (cosHalfTheta < 0.0f)
        {
            bAdjusted = Quat(-b.x, -b.y, -b.z, -b.w);
            cosHalfTheta = -cosHalfTheta;
        }
        if (cosHalfTheta > 0.9995f)
            return Nlerp(a, bAdjusted, t);

        float halfTheta = std::acos(std::clamp(cosHalfTheta, -1.0f, 1.0f));
        float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);

        float ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
        float ratioB = std::sin(t * halfTheta) / sinHalfTheta;

        return Quat(
            a.x * ratioA + bAdjusted.x * ratioB,
            a.y * ratioA + bAdjusted.y * ratioB,
            a.z * ratioA + bAdjusted.z * ratioB,
            a.w * ratioA + bAdjusted.w * ratioB
        );
    }
}