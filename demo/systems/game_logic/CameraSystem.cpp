#include "CameraSystem.h"

#include <algorithm>
#include <cmath>

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/graphics/ActiveCameraTag.h"

#include "math_custom/Mat4.h"
#include "math_custom/Vector3.h"

void CameraSystem::Initialize(SceneContext& sceneCtx)
{
    ctx = &sceneCtx;
}

void CameraSystem::Update(SceneContext&, float dt)
{
    processMouse();
    processKeyboard(dt);

    updateVectors();
    updateMatrices();
}

void CameraSystem::processKeyboard(float dt)
{
    const entt::entity camEntity = getActiveCamera();
    if (camEntity == entt::null)
        return;

    auto* camera = registry.try_get<CameraComponent>(camEntity);
    auto* transform = registry.try_get<TransformComponent>(camEntity);
    if (!camera || !transform)
        return;

    const Keyboard& keys = ctx->engine.input.Keys();
    const float velocity = movementSpeed * dt;

    if (keys.IsDown(KeyCode::W)) transform->position += camera->front * velocity;
    if (keys.IsDown(KeyCode::S)) transform->position -= camera->front * velocity;
    if (keys.IsDown(KeyCode::A)) transform->position -= camera->right * velocity;
    if (keys.IsDown(KeyCode::D)) transform->position += camera->right * velocity;
    if (keys.IsDown(KeyCode::Q)) transform->position -= camera->up * velocity;
    if (keys.IsDown(KeyCode::E)) transform->position += camera->up * velocity;
}

void CameraSystem::processMouse()
{
    const Mouse& mouse = ctx->engine.input.Cursor();
    const bool rmbDown = mouse.IsDown(MouseButton::Right);

    if (rmbDown != camMode)
    {
        ctx->engine.platform.SetCursorMode(
            rmbDown ? CursorMode::Disabled : CursorMode::Normal
        );
        camMode = rmbDown;
        firstMouseThisHold = true;
    }

    if (!camMode)
        return;

    if (firstMouseThisHold)
    {
        firstMouseThisHold = false;
        return;
    }

    const entt::entity camEntity = getActiveCamera();
    if (camEntity == entt::null)
        return;

    auto* camera = registry.try_get<CameraComponent>(camEntity);
    if (!camera)
        return;

    Vector2 delta = mouse.Delta();
    float xOffset = delta.x * mouseSensitivity;
    float yOffset = -delta.y * mouseSensitivity; 

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    camera->pitch = std::clamp(camera->pitch, -89.0f, 89.0f);

    if (camera->yaw > 360.0f) camera->yaw -= 360.0f;
    if (camera->yaw < -360.0f) camera->yaw += 360.0f;
}

void CameraSystem::updateVectors()
{
    const entt::entity camEntity = getActiveCamera();
    if (camEntity == entt::null)
        return;

    auto* camera = registry.try_get<CameraComponent>(camEntity);
    if (!camera)
        return;

    const float yawRad = Mat4::radians(camera->yaw);
    const float pitchRad = Mat4::radians(camera->pitch);

    Vector3 front;
    front.x = std::cos(yawRad) * std::cos(pitchRad);
    front.y = std::sin(pitchRad);
    front.z = std::sin(yawRad) * std::cos(pitchRad);

    camera->front = front.normalized();
    camera->right = camera->front.cross(Vector3(0.0f, 1.0f, 0.0f)).normalized();
    camera->up = camera->right.cross(camera->front).normalized();
}

void CameraSystem::updateMatrices()
{
    const entt::entity camEntity = getActiveCamera();
    if (camEntity == entt::null)
        return;

    auto* camera = registry.try_get<CameraComponent>(camEntity);
    auto* transform = registry.try_get<TransformComponent>(camEntity);
    if (!camera || !transform)
        return;

    camera->viewMatrix = Mat4::lookAt(
        transform->position,
        transform->position + camera->front,
        camera->up
    );

    camera->projectionMatrix = Mat4::perspective(
        camera->fov, camera->aspectRatio, camera->nearPlane, camera->farPlane
    );
}

entt::entity CameraSystem::getActiveCamera()
{
    auto view = registry.view<ActiveCameraTag, CameraComponent, TransformComponent>();
    for (auto entity : view)
        return entity;
    return entt::null;
}

void CameraSystem::Shutdown(SceneContext&)
{
    ctx = nullptr;
}