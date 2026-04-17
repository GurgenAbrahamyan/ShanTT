
#include "CameraSystem.h"
#include <cmath>
#include <iostream>
#include "../../ecs/components/core/TransformComponent.h"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/graphics/ActiveCameraTag.h"
#include "../../core/Event.h"
#include "../EngineContext.h"
#include "../../math_custom/Mat4.h"
#include "../../math_custom/GLAdapter.h"


CameraSystem::CameraSystem(EventBus* bus, entt::registry& registry) : registry(registry) {
    bus->subscribe<PressedKey>([this](PressedKey& event) {
        this->processKeyboard(this->registry, event.key, EngineContext::get().deltaTime);
        });

    bus->subscribe<CameraMode>([this](CameraMode& event) {
        camMode = event.key;
        });

    bus->subscribe<MouseDragged>([this](MouseDragged& event) {
        processMouse(this->registry, event.x, event.y);
        });
}

// --- Update loop ---
void CameraSystem::update(entt::registry& registry, float dt) {
    updateVectors(registry);
    updateMatrices(registry);
}

// --- Input ---
void CameraSystem::processKeyboard(entt::registry& registry, char key, float dt) {
    auto camEntity = getActiveCamera();
    if (camEntity == entt::null) return;

    auto* cam = registry.try_get<CameraComponent>(camEntity);
    auto* transform = registry.try_get<TransformComponent>(camEntity);
    if (!cam || !transform) return;

    float velocity = movementSpeed * dt;
    if (key == 'W') transform->position += cam->front * velocity;
    if (key == 'S') transform->position -= cam->front * velocity;
    if (key == 'A') transform->position += cam->right * velocity;
    if (key == 'D') transform->position -= cam->right * velocity;
    if (key == 'Q') transform->position -= cam->up * velocity;
    if (key == 'E') transform->position += cam->up * velocity;
}

void CameraSystem::processMouse(entt::registry& registry, float xoffset, float yoffset) {
    if (!camMode) return;

    auto camEntity = getActiveCamera();
    if (camEntity == entt::null) return;

    auto* cam = registry.try_get<CameraComponent>(camEntity);
    if (!cam) return;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cam->yaw -= xoffset;
    cam->pitch += yoffset;

    cam->pitch = std::clamp(cam->pitch, -89.0f, 89.0f);
    if (cam->yaw > 360.f) cam->yaw -= 360.f;
    if (cam->yaw < -360.f) cam->yaw += 360.f;
}

// --- Vector updates in engine coordinates ---
void CameraSystem::updateVectors(entt::registry& registry) {
    auto camEntity = getActiveCamera();
    if (camEntity == entt::null) return;
    auto* cam = registry.try_get<CameraComponent>(camEntity);
    if (!cam) return;

    // Convert yaw/pitch to radians
    float yawRad = Mat4::radians(cam->yaw);
    float pitchRad = Mat4::radians(cam->pitch);

    // Recompute front vector for Z-up / Y-forward
    cam->front.x = sin(yawRad) * cos(pitchRad);   // left-right
    cam->front.y = cos(yawRad) * cos(pitchRad);   // forward-back
    cam->front.z = sin(pitchRad);                 // up-down
    cam->front = cam->front.normalized();

    // Right and up
    cam->right = cam->front.cross(Vector3(0, 0, 1)).normalized(); // right perpendicular to up
    cam->up = cam->right.cross(cam->front).normalized();
}

// --- Matrices for OpenGL ---
void CameraSystem::updateMatrices(entt::registry& registry) {
    auto camEntity = getActiveCamera();
    if (camEntity == entt::null) return;

    auto* cam = registry.try_get<CameraComponent>(camEntity);
    auto* transform = registry.try_get<TransformComponent>(camEntity);
    if (!cam || !transform) return;

    // Convert engine Z-up -> OpenGL Y-up
    
    Vector3 posGL = GLAdapter::toGL(transform->position);
    Vector3 frontGL= GLAdapter::toGL(cam->front);
    Vector3 upGL = GLAdapter::toGL(cam->up);

    cam->viewMatrix = Mat4::lookAt(posGL, posGL + frontGL, upGL);
    cam->projectionMatrix = Mat4::perspective(cam->fov, cam->aspectRatio, cam->nearPlane, cam->farPlane);
	//cam->projectionMatrix = Mat4::ortho(860,-860, 540, -540, cam->nearPlane, cam->farPlane);
}

// --- Helper ---
entt::entity CameraSystem::getActiveCamera() {
    for (auto entity : registry.view<ActiveCameraTag>()) return entity;
    return entt::null;
}