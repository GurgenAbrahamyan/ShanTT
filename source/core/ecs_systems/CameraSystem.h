#pragma once

#include "../../include/EnTT/entt.hpp"
#include "../../core/EventBus.h"

class CameraSystem {
public:
    CameraSystem(EventBus* bus, entt::registry& registry);

    void update(entt::registry& registry, float dt);

private:
    void processKeyboard(entt::registry& registry, char key, float dt);
    void processMouse(entt::registry& registry, float xoffset, float yoffset);
    void updateVectors(entt::registry& registry);
    void updateMatrices(entt::registry& registry);

    // Helper to get active camera entity
    entt::entity getActiveCamera();

    bool  camMode = false;
    float movementSpeed = 10.0f;
    float mouseSensitivity = 0.1f;

    entt::registry& registry;
};