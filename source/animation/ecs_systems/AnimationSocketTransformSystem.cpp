#include "AnimationSocketTransformSystem.h"

#include "animation/AnimationPoseComponent.h"

#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/graphics/SocketComponent.h"
#include "ecs/components/physics/BoneComponent.h"

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/core/ParentComponent.h"

#include "resources/assets/Skeleton/Skeleton.h"
#include "resources/managers/SkeletonManager.h"

#include "scene/SceneContext.h"

namespace
{
    entt::entity FindRoot(
        entt::registry& registry,
        entt::entity entity)
    {
        while (registry.all_of<ParentComponent>(entity))
        {
            const auto& parent =
                registry.get<ParentComponent>(entity);

            if (parent.parent == entt::null)
                break;

            entity = parent.parent;
        }

        return entity;
    }
}

void AnimationSocketTransformSystem::Update(
    SceneContext& context,
    float)
{
    SkeletonManager& skeletonManager =
        context.engine.assets.skeletons();

    auto characterView =
        registry.view<
            SkeletonComponent,
            AnimatedPoseComponent
        >();

    auto socketView =
        registry.view<
            SocketComponent,
            BoneComponent,
            TransformComponent
        >();

    for (auto [
        character,
        skeletonComponent,
        pose
    ] : characterView.each())
    {
        Skeleton* skeleton =
            skeletonManager.getSkeleton(
                skeletonComponent.skeleton);

        if (!skeleton)
            continue;

        const size_t boneCount =
            skeleton->bones.size();

        if (pose.localPositions.size() != boneCount ||
            pose.localRotations.size() != boneCount ||
            pose.localScales.size() != boneCount)
        {
            continue;
        }

        for (auto [
            socketEntity,
            bone,
            transform
        ] : socketView.each())
        {
           
            if (FindRoot(registry, socketEntity) != character)
                continue;

            if (bone.boneIndex >= boneCount)
                continue;

            transform.position =
                pose.localPositions[bone.boneIndex];

            transform.rotation =
                pose.localRotations[bone.boneIndex];

            transform.scale =
                pose.localScales[bone.boneIndex];
        }
    }
}