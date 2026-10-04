// AnimatedPoseToTransformSystem.cpp
#include "AnimationPoseToTransform.h"

#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/core/TransformComponent.h"
#include "../AnimationPoseComponent.h"

#include "resources/managers/SkeletonManager.h"
#include "scene/SceneContext.h"

void AnimatedPoseToTransformSystem::Update(SceneContext& ctx, float dt)
{
    (void)dt;

    SkeletonManager& skelMgr = ctx.engine.assets.skeletons();

    auto view = registry.view<SkeletonComponent, AnimatedPoseComponent>();

    for (auto [entity, skelComp, pose] : view.each())
    {
        if (!skelComp.skeleton.isValid())
            continue;

        Skeleton* skeleton = skelMgr.getSkeleton(skelComp.skeleton);
        if (!skeleton)
            continue;

        const size_t boneCount = skeleton->bones.size();

        if (skelComp.boneEntities.size() != boneCount)
            continue; // not wired up for this entity, skip safely

        if (pose.localPositions.size() != boneCount ||
            pose.localRotations.size() != boneCount ||
            pose.localScales.size()    != boneCount)
        {
            continue;
        }

        for (size_t i = 0; i < boneCount; ++i)
        {
            const entt::entity boneEntity = skelComp.boneEntities[i];

            if (boneEntity == entt::null || !registry.valid(boneEntity))
                continue;

            if (!registry.all_of<TransformComponent>(boneEntity))
                continue;

            TransformComponent& transform =
                registry.get<TransformComponent>(boneEntity);

            transform.position = pose.localPositions[i];
            transform.rotation = pose.localRotations[i];
            transform.scale    = pose.localScales[i];
        }
    }
}