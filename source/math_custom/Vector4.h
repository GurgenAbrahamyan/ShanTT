#pragma once

struct Vector4 {
    float x, y, z, w;
    Vector4(float x = 1, float y = 1, float z = 1, float w = 1)
        : x(x), y(y), z(z), w(w) {
    }
    float* data() { return &x; }
    const float* data() const { return &x; }
};