#include "RagdollPoseSyncSystem.h"

#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/physics/RagDollComponent.h"
#include "ecs/components/graphics/SkeletonComponent.h"

#include "scene/SceneContext.h"

void RagdollPoseSyncSystem::Update(SceneContext& context, float)
{
    JPH::BodyInterface& bodyInterface = context.engine.physics.GetSystem().GetBodyInterface();

    registry.view<RagdollComponent, SkeletonComponent>().each(
        [&](RagdollComponent& ragdoll, SkeletonComponent& skelComp)
        {
            Skeleton* skeleton = context.engine.assets.skeletons().getSkeleton(skelComp.skeleton);
            if (!skeleton) return;
            if (skelComp.skinMatrices.size() != skeleton->bones.size())
                skelComp.skinMatrices.resize(skeleton->bones.size());

            for (auto& joint : ragdoll.joints)
            {
                JPH::RVec3 pos; JPH::Quat rot;
                bodyInterface.GetPositionAndRotation(joint.bodyId, pos, rot);
                Mat4 boneWorld = Mat4::translate(Vector3(pos.GetX(), pos.GetY(), pos.GetZ()))
                               * Mat4::fromQuat(Quat(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW()));
                skelComp.skinMatrices[joint.boneId] = boneWorld * skeleton->bones[joint.boneId].invBind;
            }
            // fallback pass for non-ragdolled bones: nearest resolved ancestor + local anim offset
        });
}