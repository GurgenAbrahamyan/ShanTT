#include "SkeletalPoseResolveExtractionSystem.h"

#include <cstdint>
#include <limits>
#include <vector>

#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/physics/BoneComponent.h"
#include "ecs/components/physics/RagdollBodyComponent.h"
#include "ecs/components/core/TransformComponent.h"
#include "animation/AnimationPoseComponent.h"

#include "resources/assets/Skeleton/Skeleton.h"

#include "math_custom/Mat4.h"
#include "math_custom/Quat.h"
#include "math_custom/Vector3.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

namespace
{
    constexpr uint32_t INVALID_BONE =
        std::numeric_limits<uint32_t>::max();


    Mat4 MakeLocalMatrix(
        const AnimatedPoseComponent& pose,
        uint32_t boneIndex)
    {
        return
            Mat4::translate(
                pose.localPositions[boneIndex]
            )
            *
            Mat4::fromQuat(
                pose.localRotations[boneIndex]
            )
            *
            Mat4::scale(
                pose.localScales[boneIndex]
            );
    }


    Mat4 GetBodyWorldTransform(
        JPH::BodyInterface& bodyInterface,
        JPH::BodyID bodyId)
    {
        JPH::RVec3 position;
        JPH::Quat rotation;

        bodyInterface.GetPositionAndRotation(
            bodyId,
            position,
            rotation
        );

        return
            Mat4::translate(
                Vector3(
                    static_cast<float>(position.GetX()),
                    static_cast<float>(position.GetY()),
                    static_cast<float>(position.GetZ())
                )
            )
            *
            Mat4::fromQuat(
                Quat(
                    rotation.GetX(),
                    rotation.GetY(),
                    rotation.GetZ(),
                    rotation.GetW()
                )
            );
    }
}


void SkeletalPoseResolveSystem::extract(
    entt::registry& registry,
    FrameRenderData&)
{
    JPH::BodyInterface& bodyInterface =
        m_bodyInterface;


    auto characterView =
        registry.view<
            SkeletonComponent,
            AnimatedPoseComponent,
            TransformComponent
        >();


    for (auto [
        character,
        skeletonComponent,
        pose,
        transform
    ] : characterView.each())
    {
        (void)character;


        // ------------------------------------------------------------
        // Skeleton
        // ------------------------------------------------------------

        if (!skeletonComponent.skeleton.isValid())
            continue;


        Skeleton* skeleton =
            assets.skeletons().getSkeleton(
                skeletonComponent.skeleton
            );


        if (!skeleton)
            continue;


        const size_t boneCount =
            skeleton->bones.size();


        // ------------------------------------------------------------
        // Validate animation pose
        // ------------------------------------------------------------

        if (pose.localPositions.size() != boneCount ||
            pose.localRotations.size() != boneCount ||
            pose.localScales.size() != boneCount)
        {
            continue;
        }


        // ------------------------------------------------------------
        // Ensure skin palette
        // ------------------------------------------------------------

        if (skeletonComponent.skinMatrices.size() != boneCount)
        {
            skeletonComponent.skinMatrices.resize(
                boneCount
            );
        }


        // ------------------------------------------------------------
        // Find physical body for every bone.
        //
        // IMPORTANT:
        //
        // Presence of RagdollBodyComponent means:
        //
        //     PHYSICS OWNS THIS BONE
        //
        // It does NOT matter whether the ECS entity is parented
        // to an animation socket.
        // ------------------------------------------------------------

        std::vector<entt::entity> physicalEntities(
            boneCount,
            entt::entity{entt::null}
        );


        auto physicalView =
            registry.view<
                BoneComponent,
                RagdollBodyComponent
            >();


        for (auto [
            entity,
            bone,
            body
        ] : physicalView.each())
        {
            if (bone.boneIndex >= boneCount)
                continue;


            if (body.bodyId.IsInvalid())
                continue;


            physicalEntities[bone.boneIndex] =
                entity;
        }


        // ------------------------------------------------------------
        // Resolve global transforms.
        //
        // global[i] is the transform that will be used to generate
        // the final skin matrix.
        // ------------------------------------------------------------

        std::vector<Mat4> global(
            boneCount
        );


        std::vector<bool> resolved(
            boneCount,
            false
        );


        // ------------------------------------------------------------
        // FIRST:
        //
        // Physical bones come directly from Jolt.
        //
        // Animation is NOT used here.
        // ------------------------------------------------------------

        for (size_t i = 0; i < boneCount; ++i)
        {
            entt::entity physicalEntity =
                physicalEntities[i];


            if (physicalEntity == entt::null)
                continue;


            const RagdollBodyComponent& body =
                registry.get<RagdollBodyComponent>(
                    physicalEntity
                );


            global[i] =
                Mat4::trs(transform.position, 
                    transform.rotation,
                    transform.scale
                    ).inverse() *
                GetBodyWorldTransform(
                    bodyInterface,
                    body.bodyId
                ) * Mat4::scale(transform.scale);


            resolved[i] = true;
        }


        // ------------------------------------------------------------
        // SECOND:
        //
        // Resolve bones which aren't physical.
        //
        // These still come from animation.
        // ------------------------------------------------------------

        for (size_t i = 0; i < boneCount; ++i)
        {
            if (resolved[i])
                continue;


            const uint32_t parent =
                skeleton->bones[i].parentId;


            const Mat4 local =
                MakeLocalMatrix(
                    pose,
                    static_cast<uint32_t>(i)
                );


            // --------------------------------------------------------
            // Root
            // --------------------------------------------------------

            if (parent == INVALID_BONE)
            {
                global[i] = local;
                resolved[i] = true;
                continue;
            }


            // --------------------------------------------------------
            // Parent already resolved?
            //
            // If parent is physical, this means this animated bone
            // follows the simulated physical hierarchy.
            //
            // If parent is animated, this is normal animation
            // hierarchy evaluation.
            // --------------------------------------------------------

            if (!resolved[parent])
                continue;


            global[i] =
                global[parent] *
                local;


            resolved[i] = true;
        }

        for (size_t i = 0; i < boneCount; ++i)
        {
            if (!resolved[i])
            {
                skeletonComponent.skinMatrices[i] =
                    Mat4{};

                continue;
            }


            skeletonComponent.skinMatrices[i] =
                global[i] *
                skeleton->bones[i].invBind;
        }
    }
}