#pragma once
#include "../../ecs/components/graphics/LightComponent.h"
#include "../../ecs/components/graphics/ShadowCasterComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../../math_custom/Mat4.h"
#include "../../math_custom/Vector3.h"

#include "core/ecs_systems/ISystem.h"
#include "ShadowAtlas.h"

#include <cmath>
#include <vector>

struct ShadowSettings {
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float orthoWidth = 20.0f;
    float orthoHeight = 20.0f;
};

class ShadowSystem : public ISystem {
public:
    ShadowSettings settings;
    ShadowAtlas atlas;

    ShadowSystem(entt::registry& reg) : ISystem(reg) {}

    void Update(SceneContext& /*ctx*/, float /*dt*/) override {
        atlas.freeAll();

        auto view = registry.view<LightComponent, TransformComponent>();
        for (auto entity : view) {
            auto& lightComp = view.get<LightComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            if (!lightComp.castsShadow) {
                registry.remove<ShadowCasterComponent>(entity);
                continue;
            }

            Vector3 pos = transform.position;
            Vector3 dir = Mat4::fromQuat(transform.rotation)
                              .multiplyVec({0.0f, 0.0f, -1.0f}, 0.0f);

            float near = (lightComp.shadowNearPlane >= 0.0f) ? lightComp.shadowNearPlane : settings.nearPlane;
            float far  = (lightComp.shadowFarPlane  >= 0.0f) ? lightComp.shadowFarPlane  : settings.farPlane;

            if (lightComp.type == LightType::Directional) {
                int tileIdx = atlas.allocateBestFit();
                if (tileIdx < 0) 
                    {registry.remove<ShadowCasterComponent>(entity); continue;}

                float orthoSize = (lightComp.shadowOrthoSize >= 0.0f) ? lightComp.shadowOrthoSize : settings.orthoWidth;
                ShadowTile& tile = atlas.getTile(tileIdx);

   
                constexpr float SUN_DISTANCE = 100.0f;
                Vector3 canonicalPos = -dir.normalized() * SUN_DISTANCE;

                ShadowCasterComponent caster;
                caster.shadowData.push_back(ShadowData{
                    tile.uvMin, tile.uvMax,
                calcDirectional(canonicalPos, dir, near, far, orthoSize)
    });
    registry.emplace_or_replace<ShadowCasterComponent>(entity, std::move(caster));
}
            else if (lightComp.type == LightType::Spot) {
                int tileIdx = atlas.allocateBestFit();
                if (tileIdx < 0) { registry.remove<ShadowCasterComponent>(entity); continue; }

                ShadowTile& tile = atlas.getTile(tileIdx);
                ShadowCasterComponent caster;
                caster.shadowData.push_back(ShadowData{
                    tile.uvMin, tile.uvMax,
                    calcSpot(pos, dir, lightComp.innerConeAngle, near, far)
                });
                registry.emplace_or_replace<ShadowCasterComponent>(entity, std::move(caster));
            }
            else if (lightComp.type == LightType::Point) {
                int tileIdx = atlas.allocateBestFitConsecutive(6);
                if (tileIdx < 0) { registry.remove<ShadowCasterComponent>(entity); continue; }

                std::vector<Mat4> faces = calcPoint(pos, near, far);
                ShadowCasterComponent caster;
                caster.shadowData.reserve(6);
                for (int f = 0; f < 6; f++) {
                    ShadowTile& tile = atlas.getTile(tileIdx + f);
                    caster.shadowData.push_back(ShadowData{ tile.uvMin, tile.uvMax, faces[f] });
                }
                registry.emplace_or_replace<ShadowCasterComponent>(entity, std::move(caster));
            }
        }
    }

private:
    Mat4 calcDirectional(Vector3 pos, Vector3 dir, float near, float far, float orthoSize) {
        Vector3 lightDir = dir.normalized();
        Vector3 center = pos + lightDir * 10.0f;
        Vector3 up = (fabs(lightDir.y) > 0.99f) ? Vector3(0, 0, 1) : Vector3(0, 1, 0);
        Mat4 view = Mat4::lookAt(pos, center, up);
        Mat4 proj = Mat4::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near, far);
        return proj * view;
    }

    Mat4 calcSpot(Vector3 pos, Vector3 dir, float innerCone, float near, float far) {
        Vector3 lightDir = dir.normalized();
        Vector3 center = pos + lightDir * 10.0f;
        Vector3 up = (fabs(lightDir.y) > 0.99f) ? Vector3(0, 0, 1) : Vector3(0, 1, 0);
        Mat4 view = Mat4::lookAt(pos, center, up);
        float fovRad = acos(innerCone) * 2.0f;
        float fovDeg = fovRad * (180.0f / 3.14159f);
        Mat4 proj = Mat4::perspective(fovDeg, 1.0f, near, far);
        return proj * view;
    }

    std::vector<Mat4> calcPoint(Vector3 pos, float near, float far) {
        std::vector<Mat4> all;
        all.push_back(calcPointFace(Vector3(1, 0, 0),  Vector3(0, -1, 0), pos, near, far));
        all.push_back(calcPointFace(Vector3(-1, 0, 0), Vector3(0, -1, 0), pos, near, far));
        all.push_back(calcPointFace(Vector3(0, 1, 0),  Vector3(0, 0, 1), pos, near, far));
        all.push_back(calcPointFace(Vector3(0, -1, 0), Vector3(0, 0, -1), pos, near, far));
        all.push_back(calcPointFace(Vector3(0, 0, 1),  Vector3(0, -1, 0), pos, near, far));
        all.push_back(calcPointFace(Vector3(0, 0, -1), Vector3(0, -1, 0), pos, near, far));
        return all;
    }

    Mat4 calcPointFace(Vector3 dir, Vector3 up, Vector3 pos, float near, float far) {
        Mat4 view = Mat4::lookAt(pos, pos + dir, up);
        Mat4 proj = Mat4::perspective(90.3f, 1.0f, near, far);
        return proj * view;
    }
};