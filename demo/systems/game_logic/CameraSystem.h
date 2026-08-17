#pragma once

#include <entt/entt.hpp>

#include "core/ecs_systems/ISystem.h"
#include "scene/SceneContext.h"
#include "input/InputTypes.h"

class CameraSystem : public ISystem
{
public:
    CameraSystem(entt::registry& reg)
        : ISystem(reg)
    {
    }

    void Initialize(SceneContext& ctx) override;
    void Update(SceneContext& ctx, float dt) override;
    void Shutdown(SceneContext& ctx) override;

private:
    void processKeyboard(float dt);
    void processMouse();
    void updateVectors();
    void updateMatrices();

    entt::entity getActiveCamera();

private:
    bool camMode = false;    
    bool firstMouseThisHold = true;

    float movementSpeed = 10.0f;
    float mouseSensitivity = 0.1f;

    SceneContext* ctx = nullptr;
};