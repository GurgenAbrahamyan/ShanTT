// MouseInput.h
#pragma once
#include "core/ecs_systems/ISystem.h"

class MouseInput : public ISystem
{
public:
    void Update(SceneContext& ctx, float dt) override;

private:
    bool cameraMode = false;
};