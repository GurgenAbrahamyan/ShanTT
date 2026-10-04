#pragma once

#include "core/ecs_systems/ISystem.h"

class PhysicsSyncSystem : public ISystem
{
public:

    PhysicsSyncSystem (entt::registry& reg ) : ISystem(reg){}
    
    void Update(SceneContext& context, float dt) override;
};