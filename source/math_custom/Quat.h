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


    Quat operator+(const Quat& r) const {
        return Quat( x + r.x, y + r.y,
                     z + r.z, w + r.w );
    }

    Quat operator-(const Quat& r) const {
        return Quat( x - r.x, y - r.y,
                     z - r.z, w - r.w );
    }

    Quat operator*(const Quat& r) const {
        float newX = w * r.x + x * r.w + y * r.z - z * r.y;
        float newY = w * r.y - x * r.z + y * r.w + z * r.x;
        float newZ = w * r.z + x * r.y - y * r.x + z * r.w;
        float newW = w * r.w - x * r.x - y * r.y - z * r.z;

        return Quat{newX, newY, newZ, newW};
    }

    Quat operator*(float scale) const {
        return Quat( x * scale, y * scale,
                     z * scale, w * scale);
    }

    Quat operator/(float scale) const {
        assert(std::abs(scale) > 1e-8f && "Division by zero or near-zero");
        return Quat( x / scale, y / scale,
                     z / scale, w / scale );
    }

    Quat& operator+=(const Quat& r) {
         x += r.x; y += r.y;
         z += r.z; w += r.w;

         return *this;
    }

    Quat& operator-=(const Quat& r) {
         x -= r.x; y -= r.y;
         z -= r.z; w -= r.w;

         return *this;
    }

    Quat& operator*=(const Quat& r){
        float newX = w * r.x + x * r.w + y * r.z - z * r.y;
        float newY = w * r.y - x * r.z + y * r.w + z * r.x;
        float newZ = w * r.z + x * r.y - y * r.x + z * r.w;
        float newW = w * r.w - x * r.x - y * r.y - z * r.z;

        x = newX;
        y = newY;
        z = newZ;
        w = newW;

        return *this;
    }

    Quat &operator*=(float scale) {
         x *= scale; y *= scale;
         z *= scale; w *= scale;
         
         return *this;
    }

    Quat &operator/=(float scale) {
        assert(std::abs(scale) > 1e-8 && "Division by zero or near-zero");
        x /= scale; y /= scale;
        z /= scale; w /= scale;

        return *this;
    }


    Quat operator-() const {
        return Quat( -x, -y, -z, -w );
    }



    bool operator==(const Quat& other ) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    
    bool operator!=(const Quat& other ) const  {
        return !(*this == other);
    }

    bool nearEqual(const Quat& other, float epsilon = 1e-5f) const {
        return std::abs(x - other.x) <= epsilon &&
               std::abs(y - other.y) <= epsilon &&
               std::abs(z - other.z) <= epsilon &&
               std::abs(w - other.w) <= epsilon ;
    }


    float length()        const { return std::sqrt( x * x + y * y + z * z + w * w); }
    float lengthSquared() const { return x * x + y * y + z * z + w * w; }

    void normalize() {
        float len = length();
        assert(len > 1e-8f && "Cannot normalize near-zero Quat");

        float inv = 1.0f / len;

        x *= inv;  y *= inv;
        z *= inv;  w *= inv;
    }

    
    Quat normalized() const {
        float len = length();
        assert(len > 1e-8f && "Cannot normalize near-zero Quat");

        return Quat(x / len, y / len, z / len, w / len);
    }

    float dot(const Quat &other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    Quat conjugate() const { return Quat(-x, -y, -z, w); }

    Quat inverse() const { return Quat{}; }

    Vector3 rotate(const Vector3& v)  const {

        Vector3 qVec(x, y, z);
        Vector3 uv = qVec.cross(v);
        Vector3 uuv = qVec.cross(uv);

        return (v + (uv * (2.0f * w))) + (uuv * (2.0f));
    }

    Vector3 inverseRotate(const Vector3 &) const { return Vector3{}; }
    
    Vector3 toEulerDeg() const {
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

  
    static Quat fromEulerDeg(const Vector3& e) {
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


    static Quat fromAxisAngleDeg(const Vector3& axis, float deg) {
        float half = deg * 3.14159265359f / 180.0f * 0.5f;
        float s = std::sin(half);
        Vector3 n = axis.normalized();
        return Quat(n.x * s, n.y * s, n.z * s, std::cos(half));
    }

    
};

inline Quat operator*(float scalar, const Quat &other) {
    return other * scalar;
};
