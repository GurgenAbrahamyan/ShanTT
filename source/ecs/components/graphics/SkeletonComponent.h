#pragma once

#include "resources/managers/SkeletonHandleTypes.h"
#include "math_custom/Mat4.h"
#include <entt/entt.hpp>

struct SkeletonComponent {
    SkeletonID skeleton;
    std::vector<Mat4> skinMatrices;
    std::vector<entt::entity> boneEntities;
};