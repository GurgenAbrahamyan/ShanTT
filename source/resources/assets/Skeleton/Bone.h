#pragma once

#include <string>
#include <cstdint>

#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"
#include "math_custom/Mat4.h"

struct Bone
{
    std::string name;
    uint32_t parentId = std::numeric_limits<uint32_t>::max();

    Vector3 pos;
    Quat rot;
    Vector3 scale{1.0f, 1.0f, 1.0f};

    Mat4 invBind;
};