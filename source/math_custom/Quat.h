#pragma once

#include <cmath>
#include "Vector3.h"
struct Quat
{
    float x;
    float y;
    float z;
    float w;

    Quat()
        : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {
    }

    Quat(float _x, float _y, float _z, float _w)
        : x(_x), y(_y), z(_z), w(_w) {
    }

    void Normalize()
    {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len == 0.0f) return;

        float inv = 1.0f / len;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
    }


    Quat operator*(const Quat& r) const
    {
        return Quat(
            w * r.w - x * r.x - y * r.y - z * r.z,
            w * r.x + x * r.w + y * r.z - z * r.y,
            w * r.y - x * r.z + y * r.w + z * r.x,
            w * r.z + x * r.y - y * r.x + z * r.w
        );
    }

    Quat operator*(float scale) const {
        return Quat(
            x * scale,
            y * scale,
            z * scale,
            w * scale
        );
    }
    Quat operator+(const Quat& r) const
    {
        return Quat(
            x + r.x,
            y + r.y,
            z + r.z,
            w + r.w
        );
    }


    Quat conjugate() {
        return Quat(-x, -y, -z, w);
    }


   

    static Quat fromAxisAngleDeg(const Vector3& axis, float deg)
    {
        float half = deg * 3.14159265359f / 180.0f * 0.5f;
        float s = std::sin(half);
        Vector3 n = axis.normalized();
        return Quat(n.x * s, n.y * s, n.z * s, std::cos(half));
    }

    Vector3 rotate(const Vector3& v)  const
    {

        Vector3 qVec(x, y, z);
        Vector3 uv = qVec.cross(v);
        Vector3 uuv = qVec.cross(uv);

        return (v + (uv * (2.0f * w))) + (uuv * (2.0f));
    }

    Quat normalized() const {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len == 0.0f) return *this;
        return Quat(x / len, y / len, z / len, w / len);
    }

  
    Vector3 toEulerDeg() const
    {
        Vector3 e;

        // pitch (X)
        float sinp = 2.0f * (w * x - z * y);
        if (std::abs(sinp) >= 1.0f)
            e.x = std::copysign(90.0f, sinp); // gimbal pole
        else
            e.x = std::asin(sinp) * (180.0f / 3.14159265359f);

        // yaw (Y)
        float siny = 2.0f * (w * y + x * z);
        float cosy = 1.0f - 2.0f * (y * y + x * x);
        e.y = std::atan2(siny, cosy) * (180.0f / 3.14159265359f);

        // roll (Z)
        float sinr = 2.0f * (w * z + y * x);
        float cosr = 1.0f - 2.0f * (z * z + y * y);
        e.z = std::atan2(sinr, cosr) * (180.0f / 3.14159265359f);

        return e;
    }

  
    static Quat fromEulerDeg(const Vector3& e)
    {
        float cx = std::cos(e.x * 3.14159265359f / 180.0f * 0.5f);
        float sx = std::sin(e.x * 3.14159265359f / 180.0f * 0.5f);
        float cy = std::cos(e.y * 3.14159265359f / 180.0f * 0.5f);
        float sy = std::sin(e.y * 3.14159265359f / 180.0f * 0.5f);
        float cz = std::cos(e.z * 3.14159265359f / 180.0f * 0.5f);
        float sz = std::sin(e.z * 3.14159265359f / 180.0f * 0.5f);

        return Quat(
            sx * cy * cz - cx * sy * sz,  // x
            cx * sy * cz + sx * cy * sz,  // y
            cx * cy * sz - sx * sy * cz,  // z
            cx * cy * cz + sx * sy * sz   // w
        ).normalized();
    }

};