#pragma once
#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
struct Vector4 {
  
    float x, y, z, w;

    Vector4()
        : Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}

    Vector4(const Vector3 &other, float w)
        : Vector4(other.x, other.y, other.z, w) {}
    
    Vector4(const Vector2 &other, float z, float w)
        : Vector4(other.x, other.y, z, w) {}

    Vector4(float x, float y, float z, float w)
        : x {x}, y {y}, z {z}, w {w} {}

    Vector4 operator+(const Vector4 &) const { return Vector4{}; }
    Vector4 operator-(const Vector4 &) { return Vector4{}; }
    Vector4 operator*(float) const { return Vector4{}; }
    Vector4 operator/(float) const { return Vector4{}; }

    Vector4& operator+=(const Vector4 &) { return *this; }
    Vector4& operator-=(const Vector4 &) { return *this; }
    Vector4& operator*=(float) { return *this; }
    Vector4& operator/=(float) { return *this; }

    Vector4 operator-() const { return Vector4{}; };



    
    bool operator==(const Vector4 &other) const {
      return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    bool operator!=(const Vector4 &other) const { return !(*this == other); }

    bool nearEqual(const Vector4 other, float epsilon = 1e-5f) const {

      return std::abs(x - other.x) <= epsilon &&
             std::abs(y - other.y) <= epsilon &&
             std::abs(z - other.z) <= epsilon &&
             std::abs(w - other.w) <= epsilon;

    }


    
    float* data() { return &x; }
    const float* data() const { return &x; }
};

inline Vector4 operator*(float, const Vector4&) { return Vector4{}; }