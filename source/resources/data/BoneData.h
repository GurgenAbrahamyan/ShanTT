#pragma once

#include <string>
#include <cstdint>

#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"
#include "math_custom/Mat4.h"

struct BoneData
{
    std::string name;

    uint32_t parentIndex = UINT32_MAX;

    Vector3 translation{0.0f, 0.0f, 0.0f};
    Quat rotation;
    Vector3 scale{1.0f, 1.0f, 1.0f};

    Mat4 inverseBindMatrix;
};