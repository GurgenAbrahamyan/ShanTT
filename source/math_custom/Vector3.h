#pragma once
#include <cmath>
#include <cassert>
class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float num) : x(num), y(num), z(num) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3(const Vector3 &other)            = default;
    Vector3 &operator=(const Vector3 &other) = default;
    Vector3(Vector3 &&other) noexcept        = default;
    Vector3 &operator=(Vector3 &&other) noexcept = default;
    
    Vector3 operator+(const Vector3& r) const { return Vector3(x + r.x, y + r.y, z + r.z); }
    Vector3 operator-(const Vector3& r) const { return Vector3(x - r.x, y - r.y, z - r.z); }
    Vector3 operator*(float s)          const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator/(float s) const {
            assert(std::abs(s) > 1e-8f && "Division by zero or near-zero");
            return Vector3(x / s, y / s, z / s); }

    
    Vector3 operator-()                 const { return Vector3(-x, -y, -z); }

    
    Vector3& operator+=(const Vector3& r) { x += r.x; y += r.y; z += r.z; return *this; }
    Vector3& operator-=(const Vector3& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
    Vector3 &operator*=(float s)          { x *= s;   y *= s;   z *= s;   return *this; }
    
    Vector3 &operator/=(float s) {
            assert(std::abs(s) > 1e-8f && "Division by zero or near-zero");
            x /= s;   y /= s;   z /= s;   return *this; }

    
    bool operator==(const Vector3& r) const { return x == r.x && y == r.y && z == r.z; }
    bool operator!=(const Vector3& r) const { return !(*this == r); }

    
    float   dot(const Vector3& r)   const { return x * r.x + y * r.y + z * r.z; }
    Vector3 cross(const Vector3& r) const {
        return Vector3(
            y * r.z - z * r.y,
            z * r.x - x * r.z,
            x * r.y - y * r.x
        );
    }

    float   lengthSquared() const { return x * x + y * y + z * z; }
    float   length()        const { return std::sqrt(lengthSquared()); }
    Vector3 normalized() const {
    float len = length();

    assert(len > 1e-8f && "Cannot normalize zero vector");

    return *this / len;
    }

    Vector3 orthogonal() const {
        float ax = std::abs(x), ay = std::abs(y), az = std::abs(z);
        if (ax <= ay && ax <= az) return Vector3(0, -z, y);
        else if (ay <= ax && ay <= az) return Vector3(-z, 0, x);
        else                           return Vector3(-y, x, 0);
    }

    bool nearEqual(const Vector3& r, float epsilon = 1e-5f) const {
        return std::abs(x - r.x) <= epsilon &&
               std::abs(y - r.y) <= epsilon &&
               std::abs(z - r.z) <= epsilon;
    }
};


inline Vector3 operator*(float s, const Vector3& v) { return v * s; }