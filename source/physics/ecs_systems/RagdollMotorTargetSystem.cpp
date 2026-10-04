#include "RagdollMotorTargetSystem.h"

#include "ecs/components/physics/RagdollBodyComponent.h"
#include "ecs/components/physics/JointComponent.h"

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/core/ParentComponent.h"

#include "ecs/components/graphics/SocketComponent.h"

#include "scene/SceneContext.h"

#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

namespace
{
    Mat4 MakeLocalMatrix(
        const TransformComponent& transform)
    {
        return
            Mat4::translate(transform.position) *
            Mat4::fromQuat(transform.rotation) *
            Mat4::scale(transform.scale);
    }


    Mat4 GetWorldMatrix(
        entt::registry& registry,
        entt::entity entity)
    {
        if (entity == entt::null)
            return Mat4{};


        if (!registry.all_of<TransformComponent>(entity))
            return Mat4{};


        const TransformComponent& transform =
            registry.get<TransformComponent>(entity);


        const Mat4 local =
            MakeLocalMatrix(transform);


        if (!registry.all_of<ParentComponent>(entity))
            return local;


        const ParentComponent& parent =
            registry.get<ParentComponent>(entity);


        if (parent.parent == entt::null)
            return local;


        return
            GetWorldMatrix(
                registry,
                parent.parent
            ) *
            local;
    }
}

void RagdollMotorTargetSystem::FixedUpdate(
    SceneContext& context,
    float dt)
{
    auto& physicsSystem =
        context.engine.physics.GetSystem();


    JPH::BodyInterface& bodyInterface =
        physicsSystem.GetBodyInterface();


    auto view =
        registry.view<
            RagdollBodyComponent,
            JointComponent,
            TransformComponent,
            ParentComponent
        >();


    for (auto [
        entity,
        body,
        joint,
        transform,
        parent
    ] : view.each())
    {
        (void)entity;


        // ============================================================
        // Validate Jolt body
        // ============================================================

        if (body.bodyId.IsInvalid())
            continue;


        // ============================================================
        // KINEMATIC FOLLOW
        // ============================================================

        if (joint.driveState ==
            JointComponent::DriveState::KinematicFollow)
        {
            if (parent.parent == entt::null)
                continue;


            const Mat4 parentWorld =
                GetWorldMatrix(
                    registry,
                    parent.parent
                );


            const Mat4 bodyLocal =
                MakeLocalMatrix(transform);


            const Mat4 desiredWorld =
                parentWorld *
                bodyLocal;


            const Vector3 position =
                desiredWorld.getTranslation();


            const Quat rotation =
                desiredWorld.getRotation();

            bodyInterface.MoveKinematic(
                body.bodyId,

                JPH::RVec3(
                    position.x,
                    position.y,
                    position.z
                ),

                JPH::Quat(
                    rotation.x,
                    rotation.y,
                    rotation.z,
                    rotation.w
                ).Normalized(),

                dt
            );
            continue;
        }


        // ============================================================
        // FREE DYNAMIC
        //
        // No animation motor. Jolt completely owns the body.
        // ============================================================

        if (joint.driveState ==
            JointComponent::DriveState::FreeDynamic)
        {
            continue;
        }


        // ============================================================
        // POWERED DYNAMIC (legacy / currently unreachable -- see header
        // comment above)
        // ============================================================

        if (joint.driveState !=
            JointComponent::DriveState::PoweredDynamic)
        {
            continue;
        }


        // ============================================================
        // Constraint must exist
        // ============================================================

        if (!joint.constraint)
            continue;


        auto* sixDof =
            static_cast<JPH::SixDOFConstraint*>(
                joint.constraint.GetPtr()
            );


        if (!sixDof)
            continue;


        // ============================================================
        // The body was created as:
        //
        //     Socket
        //        |
        //        v
        //     Body entity
        //
        // Therefore:
        //
        //     parent.parent
        //
        // IS THE ANIMATION SOCKET.
        //
        // Do NOT search for a child socket.
        // ============================================================

        const entt::entity childSocket =
            parent.parent;


        if (childSocket == entt::null)
            continue;


        if (!registry.all_of<SocketComponent>(
                childSocket))
        {
            continue;
        }


        if (!registry.all_of<TransformComponent>(
                childSocket))
        {
            continue;
        }


        // ============================================================
        // Find the parent socket.
        //
        // The socket's parent is the animation bone entity.
        //
        // Socket
        //    |
        //    +-- Parent = parent bone
        // ============================================================

        if (!registry.all_of<ParentComponent>(
                childSocket))
        {
            continue;
        }


        const ParentComponent& socketParent =
            registry.get<ParentComponent>(
                childSocket
            );


        const entt::entity parentBone =
            socketParent.parent;


        if (parentBone == entt::null)
            continue;


        if (!registry.all_of<TransformComponent>(
                parentBone))
        {
            continue;
        }


        // ============================================================
        // Animation target
        //
        // childSocket is the animated socket belonging to this
        // physical bone.
        //
        // parentBone is the animated parent bone.
        //
        // Their relative orientation is the desired animation
        // orientation for the physical joint.
        // ============================================================

        const Mat4 parentWorld =
            GetWorldMatrix(
                registry,
                parentBone
            );


        const Mat4 childSocketWorld =
            GetWorldMatrix(
                registry,
                childSocket
            );


        const Mat4 desiredRelative =
            parentWorld.inverse() *
            childSocketWorld;


        const Quat targetRotation =
            desiredRelative.getRotation();


        // ============================================================
        // Send animation target to Jolt.
        //
        // The body remains Dynamic; Jolt's motor is responsible for
        // physically reaching the animation pose (not MoveKinematic).
        // ============================================================

        sixDof->SetTargetOrientationCS(
            JPH::Quat(
                targetRotation.x,
                targetRotation.y,
                targetRotation.z,
                targetRotation.w
            ).Normalized()
        );
    }
}