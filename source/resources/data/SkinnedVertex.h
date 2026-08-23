#pragma once

#include <cstdint>

#include "math_custom/Vector2.h"
#include "math_custom/Vector3.h"

struct SkinnedVertex
{
    Vector3 pos;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
    float   tangentW;

    uint32_t boneIds[4]  {};
    float    weights [4] {};  
};