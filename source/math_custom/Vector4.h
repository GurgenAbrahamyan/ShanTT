#pragma once

struct Vector4 {
    float x, y, z, w;
    Vector4(float x = 1, float y = 1, float z = 1, float w = 1)
        : x(x), y(y), z(z), w(w) {}

    bool operator==(const Vector4 &) const {  return false;  }
    bool operator!=(const Vector4 &) const { return false; }

    bool nearEqual(const Vector4, float) { return false; }


    
    float* data() { return &x; }
    const float* data() const { return &x; }
};