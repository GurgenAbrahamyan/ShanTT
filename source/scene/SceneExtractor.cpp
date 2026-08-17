#include "SceneExtractor.h"
#include "SceneRenderData.h"

#include "ecs/components/graphics/ActiveCameraTag.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/graphics/ModelComponent.h"
#include "ecs/components/graphics/LightComponent.h"
#include "ecs/components/graphics/CubeMapComponent.h"
#include "ecs/components/core/ParentComponent.h"
#include "ecs/components/graphics/ShadowCasterComponent.h"

#include "../render/data/GPULight.h"
#include "resources/data/RenderMaterialData.h"

#include "../math_custom/Mat4.h"
#include "../math_custom/Vector3.h"

#include <algorithm>
#include <cmath>


void SceneExtractor::extract(
    entt::registry& registry,
    FrameRenderData& out)
{
    SceneRenderData& sceneData = out.Emplace<SceneRenderData>();

    sceneData.camera = nullptr;
    sceneData.cameraTransform = nullptr;

    for (auto entity : registry.view<ActiveCameraTag>())
    {
        sceneData.camera =
            registry.try_get<CameraComponent>(entity);

        sceneData.cameraTransform =
            registry.try_get<TransformComponent>(entity);

        break;
    }

    sceneData.batches.clear();

    auto view = registry.view<TransformComponent, ModelComponent>();

    for (auto entity : view)
    {
        const auto& modelComponent = view.get<ModelComponent>(entity);

        if (!modelComponent.asset)
            continue;

        const Mat4 entityWorld = getWorldTransform(entity, registry);

        for (const auto& submesh : modelComponent.asset->meshes)
        {
            const auto& meshHandle = submesh.mesh;
            const auto& materialHandle = submesh.material;

            if (!meshHandle.isValid() || !materialHandle.isValid())
                continue;

            RenderMesh* mesh =
                m_meshManager.getMesh(meshHandle);

            const Material* material =
                m_materialManager.getMaterial(materialHandle);

            if (!mesh || !material)
                continue;

            RenderMaterial renderMaterial;

            renderMaterial.albedo =
                m_textureManager.getTexture(
                    material->GetTexture(MaterialSlot::Albedo));

            renderMaterial.arm =
                m_textureManager.getTexture(
                    material->GetTexture(MaterialSlot::ARM));

            renderMaterial.normal =
                m_textureManager.getTexture(
                    material->GetTexture(MaterialSlot::Normal));

            renderMaterial.emissive =
                m_textureManager.getTexture(
                    material->GetTexture(MaterialSlot::Emissive));

            renderMaterial.metallic = material->metallic;
            renderMaterial.roughness = material->roughness;
            renderMaterial.ao = material->ao;
            renderMaterial.baseColorFactor = material->baseColorFactor;
            renderMaterial.emissiveFactor = material->emissiveFactor;

            auto& meshMap = sceneData.batches[materialHandle];
            auto& batch = meshMap[mesh];

            auto& materialData = sceneData.materials[materialHandle];
            materialData = renderMaterial;

            batch.instances.push_back(
                entityWorld * submesh.localTransform
            );
        }
    }

    sceneData.lights.clear();
    sceneData.shadowData.clear();

    float sunElevation = 1.0f;

    registry
        .view<LightComponent, TransformComponent>()
        .each(
            [&](
                entt::entity entity,
                LightComponent& lightComponent,
                TransformComponent& transformComponent)
            {
                GPULight light{};

                light.type = static_cast<int>(lightComponent.type);
                light.shadowIndex = -1; // default: no shadow

                light.intensity = lightComponent.intensity;
                light.color = lightComponent.color;
                light.position = transformComponent.position;

                light.direction =
                    Mat4::fromQuat(
                        transformComponent.rotation
                    ).multiplyVec(
                        {0.0f, 0.0f, -1.0f},
                        0.0f
                    );

                constexpr float SUN_DISTANCE = 100.0f;

                if (lightComponent.type == LightType::Directional)
                {
                    light.position =
                    {
                        -light.direction.x * SUN_DISTANCE,
                        -light.direction.y * SUN_DISTANCE,
                        -light.direction.z * SUN_DISTANCE
                    };

                    float elevation = -light.direction.y;
                    float t = std::max(0.0f, elevation);

                    light.intensity =
                        lightComponent.intensity *
                        (t * t * std::sqrt(t));

                    sunElevation = t;
                }

                light.innerCone = lightComponent.innerConeAngle;
                light.outerCone = lightComponent.outerConeAngle;

                if (lightComponent.castsShadow)
                {
                    if (auto* caster = registry.try_get<ShadowCasterComponent>(entity))
                    {
                        light.shadowIndex = static_cast<int>(sceneData.shadowData.size());

                        for (const auto& sd : caster->shadowData)
                            sceneData.shadowData.push_back(sd);
                    }
                }

                sceneData.lights.push_back(light);
            }
        );

    auto skyView = registry.view<CubeMapComponent>();

    if (!skyView.empty())
    {
        auto cubeMapComp = &registry.get<CubeMapComponent>(skyView.front());
        cubeMapComp->dirLightInfluence = std::sqrt(sunElevation);
    }
}


Mat4 SceneExtractor::getWorldTransform(
    entt::entity entity,
    entt::registry& registry)
{
    Mat4 local;

    if (auto* transform =
        registry.try_get<TransformComponent>(entity))
    {
        local =
            Mat4::translate(transform->position) *
            Mat4::fromQuat(transform->rotation) *
            Mat4::scale(transform->scale);
    }


    if (auto* parent =
        registry.try_get<ParentComponent>(entity))
    {
        if (registry.valid(parent->parent))
        {
            return
                getWorldTransform(parent->parent, registry) *
                local;
        }
    }


    return local;
};