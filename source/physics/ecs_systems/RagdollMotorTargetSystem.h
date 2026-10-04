#pragma once

#include <entt/entt.hpp>

#include "core/ecs_systems/ISystem.h"

class RagdollMotorTargetSystem : public ISystem
{
public:
    explicit RagdollMotorTargetSystem(entt::registry& registry)
        : ISystem(registry)
    {}

    void FixedUpdate(SceneContext& ctx, float dt) override;
};