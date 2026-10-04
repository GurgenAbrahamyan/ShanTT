#pragma once

#include "core/ecs_systems/ISystem.h"

class RagdollPoseSyncSystem : public ISystem {

    public: 
        RagdollPoseSyncSystem(entt::registry& reg) : ISystem(reg){};
        void Update(SceneContext&, float) override;
};