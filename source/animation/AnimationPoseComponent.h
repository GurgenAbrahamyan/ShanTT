#pragma once
#include <vector>
#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"

struct AnimatedPoseComponent
{
    std::vector<Vector3> localPositions;
    std::vector<Quat>    localRotations;
    std::vector<Vector3> localScales;

    void resize(size_t boneCount)
    {
        localPositions.resize(boneCount);
        localRotations.resize(boneCount);
        localScales.resize(boneCount);
    }
};