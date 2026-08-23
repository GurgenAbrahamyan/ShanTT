#include "SceneExtractor.h"
#include "SceneRenderData.h"

#include "ecs/components/graphics/ActiveCameraTag.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/graphics/LightComponent.h"
#include "ecs/components/graphics/CubeMapComponent.h"
#include "ecs/components/graphics/Renderable.h"
#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/core/ParentComponent.h"
#include "ecs/components/graphics/ShadowCasterComponent.h"
#include "ecs/components/graphics/SkeletonComponent.h"

#include "../render/data/GPULight.h"
#include "resources/data/RenderMaterialData.h"

#include "../math_custom/Mat4.h"
#include "../math_custom/Vector3.h"

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <unordered_map>


void SceneExtractor::extract(
    entt::registry& registry,
    FrameRenderData& out)
{
    SceneRenderData& sceneData =
        out.Emplace<SceneRenderData>();

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

    sceneData.staticBatches.clear();
    sceneData.skinnedBatches.clear();
    sceneData.materials.clear();
    sceneData.skinMatrices.clear();

    std::unordered_map<entt::entity, uint32_t> paletteOffsetByOwner;

    registry.view<SkeletonComponent>().each(
        [&](entt::entity owner, SkeletonComponent& skelComp)
        {
            if (!skelComp.skeleton.isValid())
                return;

            const Skeleton* skeleton =
                m_skeletonManager.getSkeleton(skelComp.skeleton);

            if (!skeleton)
                return;

            const uint32_t paletteOffset =
                static_cast<uint32_t>(
                    sceneData.skinMatrices.size()
                );

            for (uint32_t i = 0; i < skeleton->bones.size(); ++i)
            {
                const Bone& bone = skeleton->bones[i];
                Mat4 global =
                    boneGlobalMatrix(*skeleton, i);

                Mat4 skinMatrix =
                    global * bone.invBind;

                sceneData.skinMatrices.push_back(
                    skinMatrix
                );
            }

            paletteOffsetByOwner[owner] = paletteOffset;
        }
    );

    auto view =
        registry.view<
            TransformComponent,
            RenderableComponent
        >();

    for (auto entity : view)
    {
        const auto& renderer =
            view.get<RenderableComponent>(entity);

        if (!renderer.visible)
            continue;

        if (!renderer.material.isValid())
            continue;

        const Mat4 entityWorld =
            getWorldTransform(entity, registry);

        const Material* material =
            m_materialManager.getMaterial(renderer.material);

        if (!material)
            continue;

        if (!sceneData.materials.contains(renderer.material))
        {
            RenderMaterial renderMaterial;

            renderMaterial.albedo =
                m_textureManager.getTexture(
                    material->GetTexture(
                        MaterialSlot::Albedo
                    )
                );

            renderMaterial.arm =
                m_textureManager.getTexture(
                    material->GetTexture(
                        MaterialSlot::ARM
                    )
                );

            renderMaterial.normal =
                m_textureManager.getTexture(
                    material->GetTexture(
                        MaterialSlot::Normal
                    )
                );

            renderMaterial.emissive =
                m_textureManager.getTexture(
                    material->GetTexture(
                        MaterialSlot::Emissive
                    )
                );

            renderMaterial.metallic =
                material->metallic;

            renderMaterial.roughness =
                material->roughness;

            renderMaterial.ao =
                material->ao;

            renderMaterial.baseColorFactor =
                material->baseColorFactor;

            renderMaterial.emissiveFactor =
                material->emissiveFactor;

            sceneData.materials.emplace(
                renderer.material,
                renderMaterial
            );
        }

        std::visit(
            [&](auto meshID)
            {
                using ID =
                    std::decay_t<decltype(meshID)>;

                if (!meshID.isValid())
                    return;

                if constexpr (
                    std::is_same_v<ID, StaticMeshID>
                )
                {
                    auto* mesh =
                        m_meshManager.getStaticMesh(meshID);

                    if (!mesh)
                        return;

                    auto& batch =
                        sceneData
                            .staticBatches[renderer.material][mesh];

                    batch.instances.push_back(
                        entityWorld
                    );
                }

                else if constexpr (std::is_same_v<ID, SkinnedMeshID>)
                {
                    auto* mesh = m_meshManager.getSkinnedMesh(meshID);
                    if (!mesh) return;

                    entt::entity owner = findSkeletonOwner(entity, registry);
                    
                    if (owner == entt::null) return;

                    auto offsetIt = paletteOffsetByOwner.find(owner);
                    if (offsetIt == paletteOffsetByOwner.end()) return;

                    //const Mat4 ownerWorld = getWorldTransform(owner, registry);
                    const Mat4 entityWorld = getWorldTransform(entity, registry);
                    auto& batch = sceneData.skinnedBatches[renderer.material][mesh];

                    batch.instances.push_back({
                      //  ownerWorld,        
                        entityWorld,  
                        offsetIt->second
                    });
                } 

                },
            renderer.mesh
        );
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

                light.type =
                    static_cast<int>(lightComponent.type);

                light.shadowIndex = -1;

                light.intensity =
                    lightComponent.intensity;

                light.color =
                    lightComponent.color;

                light.position =
                    transformComponent.position;

                light.direction =
                    Mat4::fromQuat(
                        transformComponent.rotation
                    ).multiplyVec(
                        {0.0f, 0.0f, -1.0f},
                        0.0f
                    );

                constexpr float SUN_DISTANCE = 100.0f;

                if (lightComponent.type ==
                    LightType::Directional)
                {
                    light.position =
                    {
                        -light.direction.x * SUN_DISTANCE,
                        -light.direction.y * SUN_DISTANCE,
                        -light.direction.z * SUN_DISTANCE
                    };

                    float elevation =
                        -light.direction.y;

                    float t =
                        std::max(0.0f, elevation);

                    light.intensity =
                        lightComponent.intensity *
                        (t * t * std::sqrt(t));

                    sunElevation = t;
                }

                light.innerCone =
                    lightComponent.innerConeAngle;

                light.outerCone =
                    lightComponent.outerConeAngle;

                if (lightComponent.castsShadow)
                {
                    if (auto* caster =
                        registry.try_get<
                            ShadowCasterComponent
                        >(entity))
                    {
                        light.shadowIndex =
                            static_cast<int>(
                                sceneData.shadowData.size()
                            );

                        for (const auto& sd :
                             caster->shadowData)
                        {
                            sceneData.shadowData.push_back(sd);
                        }
                    }
                }

                sceneData.lights.push_back(light);
            }
        );

    auto skyView =
        registry.view<CubeMapComponent>();

    if (!skyView.empty())
    {
        auto* cubeMapComp =
            &registry.get<CubeMapComponent>(
                skyView.front()
            );

        cubeMapComp->dirLightInfluence =
            std::sqrt(sunElevation);
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
                getWorldTransform(
                    parent->parent,
                    registry
                ) * local;
        }
    }

    return local;
}

entt::entity SceneExtractor::findSkeletonOwner(
    entt::entity entity,
    entt::registry& registry)
{
    if (registry.all_of<SkeletonComponent>(entity))
        return entity;

    if (auto* parent =
        registry.try_get<ParentComponent>(entity))
    {
        if (registry.valid(parent->parent))
        {
            return
                findSkeletonOwner(
                    parent->parent,
                    registry
                );
        }
    }

    return entt::null;
}

Mat4 SceneExtractor::boneGlobalMatrix(
    const Skeleton& skeleton,
    uint32_t boneId)
{
    const Bone& bone =
        skeleton.bones[boneId];

    const Mat4 local =
        Mat4::translate(bone.pos) *
        Mat4::fromQuat(bone.rot) *
        Mat4::scale(bone.scale);

    if (bone.parentId ==
        std::numeric_limits<uint32_t>::max())
    {
        return local;
    }

    return
        boneGlobalMatrix(
            skeleton,
            bone.parentId
        ) * local;
}