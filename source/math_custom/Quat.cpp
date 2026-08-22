#include "Mat4.h"
#include "Quat.h"
Quat Quat::fromMat4(const Mat4& m)
{
    float m00 = m(0,0), m01 = m(0,1), m02 = m(0,2);
    float m10 = m(1,0), m11 = m(1,1), m12 = m(1,2);
    float m20 = m(2,0), m21 = m(2,1), m22 = m(2,2);

    float trace = m00 + m11 + m22;
    Quat q;

    if (trace > 0.0f)
    {
        float s = 0.5f / std::sqrt(trace + 1.0f);
        q.w = 0.25f / s;
        q.x = (m21 - m12) * s;
        q.y = (m02 - m20) * s;
        q.z = (m10 - m01) * s;
    }
    else if (m00 > m11 && m00 > m22)
    {
        float s = 2.0f * std::sqrt(1.0f + m00 - m11 - m22);
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    }
    else if (m11 > m22)
    {
        float s = 2.0f * std::sqrt(1.0f + m11 - m00 - m22);
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    }
    else
    {
        float s = 2.0f * std::sqrt(1.0f + m22 - m00 - m11);
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    return q.normalized();
}