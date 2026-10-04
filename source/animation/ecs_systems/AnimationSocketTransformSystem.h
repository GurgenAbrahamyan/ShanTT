#pragma once

#include <entt/entt.hpp>

#include "core/ecs_systems/ISystem.h"

class AnimationSocketTransformSystem : public ISystem
{
public:
    explicit AnimationSocketTransformSystem(
        entt::registry& registry)
        : ISystem(registry)
    {}

    void Update(SceneContext& ctx, float dt) override;
};