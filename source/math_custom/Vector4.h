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

    Vector4 operator+(const Vector4 &other) const {
      return Vector4(x + other.x, y + other.y,
                     z + other.z, w + other.w);
    }
    
    Vector4 operator-(const Vector4 &other) const {
      return Vector4(x - other.x, y - other.y,
                     z - other.z, w - other.w);
    }
    Vector4 operator*(float scalar) const {
      return Vector4(x * scalar, y * scalar,
                     z * scalar, w * scalar);
    }

    Vector4 operator/(float scalar) const {
      assert(scalar > 1e-8f && "Division by zero or near-zero");
      
      return Vector4(x / scalar, y / scalar,
                     z / scalar, w / scalar);
    }

    Vector4 &operator+=(const Vector4 &other) {
      x += other.x;   y += other.y;
      z += other.z;   w += other.w;
      return *this;
    }
    
    Vector4 &operator-=(const Vector4 &other) {
      x -= other.x;   y -= other.y;
      z -= other.z;   w -= other.w;
      return *this;
    }

    Vector4 &operator*=(float scalar) {
      x *= scalar;   y *= scalar;
      z *= scalar;   w *= scalar;
      return *this;
    }

    Vector4 &operator/=(float scalar) {

      assert(scalar > 1e-8f && "Division by zero or near-zero");
      
      x /= scalar;   y /= scalar;
      z /= scalar;   w /= scalar;
      return *this;
    }

    Vector4 operator-() const { return Vector4(-x, -y, -z, -w); };



    
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

    float length()        { return 0.0f; }
    float lengthSquared() { return 0.0f; }

    float dot(const Vector4 &) { return 0.0f; }
    Vector4 normalized() { return Vector4(); }
    
    float* data() { return &x; }
    const float* data() const { return &x; }
};

inline Vector4 operator*(float scalar, const Vector4 &other) {
        return Vector4(other.x * scalar, other.y * scalar,
                       other.z * scalar, other.w * scalar);
  }