#pragma once

#include "core/ecs_systems/ISystem.h"

class SkeletonAnimationPoseSystem : public ISystem
{
public:
    explicit SkeletonAnimationPoseSystem(entt::registry& registry)
        : ISystem(registry)
    {}

    void Update(SceneContext& ctx, float dt) override;
};