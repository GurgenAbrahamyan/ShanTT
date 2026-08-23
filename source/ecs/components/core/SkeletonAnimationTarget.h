#pragma once

#include "resources/managers/SkeletonHandleTypes.h"

struct SkeletalAnimationTarget {
    SkeletonID skeleton;
    std::vector<int> trackToJoint;
};