#pragma once

#include <entt/entt.hpp>
#include "core/ecs_systems/ISystem.h"
#include "scene/SceneContext.h"

class CameraSystem : public ISystem
{
public:
    void Initialize(SceneContext& ctx) override;
    void Update(SceneContext& ctx, float dt) override;
    void Shutdown(SceneContext& ctx) override;
private:
    void processKeyboard(entt::registry& registry, char key, float dt);
    void processMouse(entt::registry& registry, float xoffset, float yoffset);
    void updateVectors(entt::registry& registry);
    void updateMatrices(entt::registry& registry);

    entt::entity getActiveCamera(entt::registry& registry);

    bool  camMode = false;
    float movementSpeed = 10.0f;
    float mouseSensitivity = 0.1f;

    SceneContext* ctx = nullptr; 
};