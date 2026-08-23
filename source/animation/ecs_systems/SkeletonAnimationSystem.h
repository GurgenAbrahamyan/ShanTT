#pragma once
#include "core/ecs_systems/ISystem.h"

class SkeletonAnimationSystem : public ISystem
{
public:
    explicit SkeletonAnimationSystem(entt::registry& registry)
        : ISystem(registry)
    {}

    void Update(SceneContext& ctx, float dt) override;
};