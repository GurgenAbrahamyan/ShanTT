#pragma once
#include <cmath>

struct Vector4 {
    float x, y, z, w;
    Vector4(float x = 1, float y = 1, float z = 1, float w = 1)
        : x(x), y(y), z(z), w(w) {}

    bool operator==(const Vector4 &other) const {
      return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    bool operator!=(const Vector4 &other) const { return !(*this == other); }

    bool nearEqual(const Vector4 other, float epsilon) const {

      return std::abs(x - other.x) <= epsilon &&
             std::abs(y - other.y) <= epsilon &&
             std::abs(z - other.z) <= epsilon &&
             std::abs(w - other.w) <= epsilon;

    }


    
    float* data() { return &x; }
    const float* data() const { return &x; }
};