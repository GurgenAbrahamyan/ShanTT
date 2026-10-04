#pragma once
#include <vector>
#include <cstdint>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"

struct PhysicsPoseComponent
{
    struct BoneOverride
    {
        uint32_t    boneId;
        JPH::BodyID bodyId;
        Vector3     localPos;
        Quat        localRot;
        float       blendWeight = 0.0f;
        float       blendRate   = 4.0f;
        bool        motorActive = true;
    };

    std::vector<BoneOverride> overrides;
};