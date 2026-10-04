// AnimatedPoseToTransformSystem.h
#pragma once

#include "core/ecs_systems/ISystem.h"   // whatever your base System header actually is

class AnimatedPoseToTransformSystem : public ISystem
{
public:
    AnimatedPoseToTransformSystem (entt::registry& reg) : ISystem(reg){}
    void Update(SceneContext& ctx, float dt) override;
};