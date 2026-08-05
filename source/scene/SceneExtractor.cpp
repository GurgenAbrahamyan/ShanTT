// SceneExtractor.cpp
#include "SceneExtractor.h"

#include "../ecs/components/graphics/ActiveCameraTag.h"
#include "../ecs/components/graphics/CameraComponent.h"
#include "../ecs/components/core/TransformComponent.h"
#include "../ecs/components/graphics/ModelComponent.h"
#include "../ecs/components/graphics/LightComponent.h"
#include "../ecs/components/graphics/CubeMapComponent.h"
#include "../ecs/components/core/ParentComponent.h"
#include "../render/data/GPULight.h"
#include "../math_custom/Mat4.h"
#include "../math_custom/Vector3.h"
#include <algorithm>
#include <cmath>

void SceneExtractor::extract(entt::registry& registry, FrameRenderData& frameData)
{
    frameData.camera = nullptr;
    frameData.cameraTransform = nullptr;
    frameData.cubeMapComp = nullptr;

    for (auto entity : registry.view<ActiveCameraTag>()) {
        frameData.camera = registry.try_get<CameraComponent>(entity);
        frameData.cameraTransform = registry.try_get<TransformComponent>(entity);
        break;
    }

    frameData.batches.clear();
    auto view = registry.view<TransformComponent, ModelComponent>();
    for (auto entity : view) {
        for (auto submesh : view.get<ModelComponent>(entity).asset->meshes) {
            Mat4 entityWorld = getWorldTransform(entity, registry);
            auto& meshComp = submesh.mesh;
            auto& matComp = submesh.material;
            if (!meshComp || !matComp) continue;
            Mat4 model = submesh.localTransform;
            auto& meshMap = frameData.batches[matComp];
            auto& batch = meshMap[meshComp];
            batch.instances.push_back(entityWorld * model);
        }
    }

    frameData.lights.clear();
    int shadowIndex = 0;
    float sunElevation = 1.0f;
    registry.view<LightComponent, TransformComponent>().each(
        [&]([[maybe_unused]] entt::entity entity, LightComponent& lc, TransformComponent& tc) {
            GPULight l{};
            l.type = static_cast<int>(lc.type);
            if (lc.castsShadow) {
                l.shadowIndex = shadowIndex;
                shadowIndex += (lc.type == LightType::Point) ? 6 : 1;
            }
            l.intensity = lc.intensity;
            l.color = lc.color;
            l.position = tc.position;
            l.direction = Mat4::fromQuat(tc.rotation).multiplyVec({0.0f, 0.0f, -1.0f}, 0.0f);

            constexpr float SUN_DISTANCE = 100.0f;
            if (lc.type == LightType::Directional) {
                l.position = Vector3(-l.direction.x * SUN_DISTANCE,
                                      -l.direction.y * SUN_DISTANCE,
                                      -l.direction.z * SUN_DISTANCE);
                float elevation = -l.direction.y;
                float t = std::max(0.0f, elevation);
                l.intensity = lc.intensity * (t * t * std::sqrt(t));
                sunElevation = t;
            }

            l.innerCone = lc.innerConeAngle;
            l.outerCone = lc.outerConeAngle;
            frameData.lights.push_back(l);
        });

    auto skyView = registry.view<CubeMapComponent>();
    if (!skyView.empty()) {
        frameData.cubeMapComp = &registry.get<CubeMapComponent>(skyView.front());
        frameData.cubeMapComp->dirLightInfluence = std::sqrt(sunElevation);
    }
}

Mat4 SceneExtractor::getWorldTransform(entt::entity entity, entt::registry& registry)
{
    Mat4 local = Mat4();
    if (auto* tc = registry.try_get<TransformComponent>(entity)) {
        local = Mat4::translate(tc->position) * Mat4::fromQuat(tc->rotation) * Mat4::scale(tc->scale);
    }
    if (auto* parentComp = registry.try_get<ParentComponent>(entity)) {
        if (registry.valid(parentComp->parent)) {
            return getWorldTransform(parentComp->parent, registry) * local;
        }
    }
    return local;
}