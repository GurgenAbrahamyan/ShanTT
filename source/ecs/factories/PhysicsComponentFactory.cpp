#include "PhysicsComponentFactory.h"

#include <iostream>
#include <unordered_map>
#include <limits>
#include <utility>

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/core/ParentComponent.h"
#include "ecs/components/core/HealthComponent.h"

#include "ecs/components/physics/BoneComponent.h"
#include "ecs/components/physics/JointComponent.h"

#include "ecs/components/physics/RagdollBodyComponent.h"


#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/MotionType.h>

#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>


namespace
{
    constexpr uint32_t INVALID_BONE =
        std::numeric_limits<uint32_t>::max();

    int FindBone(
        const Skeleton& skeleton,
        const std::string& name)
    {
        for (uint32_t i = 0;
             i < skeleton.bones.size();
             ++i)
        {
            if (skeleton.bones[i].name == name)
                return static_cast<int>(i);
        }

        return -1;
    }

    Mat4 GetBoneGlobal(
        const Skeleton& skeleton,
        uint32_t boneId)
    {
        Mat4 result =
            Mat4::trs(
                skeleton.bones[boneId].pos,
                skeleton.bones[boneId].rot,
                skeleton.bones[boneId].scale
            );

        uint32_t parent =
            skeleton.bones[boneId].parentId;

        while (parent != INVALID_BONE)
        {
            const Bone& parentBone =
                skeleton.bones[parent];

            Mat4 parentLocal =
                Mat4::trs(
                    parentBone.pos,
                    parentBone.rot,
                    parentBone.scale
                );

            result =
                parentLocal * result;

            parent =
                parentBone.parentId;
        }

        return result;
    }

    JPH::RefConst<JPH::Shape> CreateShape(
        const RagdollBodyDef& def)
    {
        switch (def.shape.type)
        {
            case RagdollShapeType::Capsule:
            {
                return new JPH::CapsuleShape(
                    def.shape.halfHeight,
                    def.shape.radius
                );
            }

            case RagdollShapeType::Box:
            {
                return new JPH::BoxShape(
                    JPH::Vec3(
                        def.shape.halfExtent.x,
                        def.shape.halfExtent.y,
                        def.shape.halfExtent.z
                    )
                );
            }

            case RagdollShapeType::Sphere:
            {
                return new JPH::SphereShape(
                    def.shape.radius
                );
            }

            default:
                return nullptr;
        }
    }

    JPH::Quat ToJoltQuat(const Quat& q)
    {
        return JPH::Quat(
            q.x,
            q.y,
            q.z,
            q.w
        );
    }


    JPH::RVec3 ToJoltPosition(const Vector3& v)
    {
        return JPH::RVec3(
            v.x,
            v.y,
            v.z
        );
    }
}


namespace PhysicsComponentFactory
{

TransformComponent createTransform(
    Vector3 translation,
    Quat rotation,
    Vector3 scale)
{
    TransformComponent transform;

    transform.position = translation;
    transform.rotation = rotation;
    transform.scale = scale;

    return transform;
}


RigidBodyComponent createRigidBody(
    entt::registry& registry,
    entt::entity entity,
    Vector3 translation,
    Quat rotation,
    Vector3 scale,
    float mass)
{
    RigidBodyComponent rb;

    rb.mass = mass;

    rb.invmass =
        mass > 0.0f
            ? 1.0f / mass
            : 0.0f;

    rb.linearVelocity =
        Vector3(0, 0, 0);

    rb.angularVelocity =
        Vector3(0, 0, 0);

    rb.forceAccum =
        Vector3(0, 0, 0);

    if (!registry.all_of<TransformComponent>(entity))
    {
        registry.emplace<TransformComponent>(
            entity,
            createTransform(
                translation,
                rotation,
                scale
            )
        );
    }

    return rb;
}


RigidBodyComponent createStaticBody(
    entt::registry& registry,
    entt::entity entity)
{
    return createRigidBody(
        registry,
        entity
    );
}


CollisionShapeComponent createCubeShape(
    Vector3 scale,
    Vector3 localPosition,
    Quat localRotation,
    Vector3 localScale)
{
    const float hw =
        scale.x * 0.5f;

    const float hd =
        scale.y * 0.5f;

    const float hh =
        scale.z * 0.5f;

    CollisionShapeComponent shape;

    shape.vertices =
    {
        Vector3(-hw, -hd, -hh),
        Vector3( hw, -hd, -hh),
        Vector3( hw,  hd, -hh),
        Vector3(-hw,  hd, -hh),

        Vector3(-hw, -hd,  hh),
        Vector3( hw, -hd,  hh),
        Vector3( hw,  hd,  hh),
        Vector3(-hw,  hd,  hh)
    };

    shape.indices =
    {
        0, 1, 2,
        2, 3, 0,

        4, 6, 5,
        6, 4, 7,

        0, 1, 5,
        5, 4, 0,

        2, 3, 7,
        7, 6, 2,

        0, 3, 7,
        7, 4, 0,

        1, 2, 6,
        6, 5, 1
    };

    shape.localPosition =
        localPosition;

    shape.localRotation =
        localRotation;

    shape.localScale =
        localScale;

    return shape;
}


CollisionShapeComponent createCustomShape(
    std::vector<Vector3> vertices,
    std::vector<unsigned int> indices,
    Vector3 localPosition,
    Quat localRotation,
    Vector3 localScale)
{
    CollisionShapeComponent shape;

    shape.vertices =
        std::move(vertices);

    shape.indices =
        std::move(indices);

    shape.localPosition =
        localPosition;

    shape.localRotation =
        localRotation;

    shape.localScale =
        localScale;

    return shape;
}


void BuildRagdoll(
    entt::registry& registry,
    const SpawnedModel& model,
    const RagdollAsset& asset,
    Skeleton& skeleton,
    SceneContext& context)
{
    auto& physicsSystem =
        context.engine.physics.GetSystem();

    JPH::BodyInterface& bodyInterface =
        physicsSystem.GetBodyInterface();

    if (model.sockets.size() != skeleton.bones.size())
    {
        std::cerr
            << "Ragdoll: SpawnedModel socket count does not "
               "match skeleton bone count\n";

        return;
    }



    std::unordered_map<uint32_t, entt::entity>
        boneToBodyEntity;

    std::unordered_map<uint32_t, JPH::BodyID>
        boneToBody;

    std::unordered_map<uint32_t, Mat4>
        boneToBodyWorld;


    for (const RagdollBodyDef& def : asset.bodies)
    {
        const int boneIndex =
            FindBone(
                skeleton,
                def.boneName
            );

        if (boneIndex < 0)
        {
            std::cerr
                << "Ragdoll: bone not found: "
                << def.boneName
                << '\n';

            continue;
        }

        const uint32_t boneId =
            static_cast<uint32_t>(boneIndex);

        const entt::entity socketEntity =
            model.sockets[boneId];

        if (
            socketEntity == entt::null ||
            !registry.valid(socketEntity)
        )
        {
            std::cerr
                << "Ragdoll: missing socket for bone "
                << def.boneName
                << '\n';

            continue;
        }

        const Mat4 boneWorld =
            GetBoneGlobal(
                skeleton,
                boneId
            );

        const Mat4 bodyWorld =
            boneWorld *
            Mat4::translate(
                def.localOffset
            ) *
            Mat4::fromQuat(
                def.localRotationOffset
            );

        boneToBodyWorld[boneId] =
            bodyWorld;


        const Vector3 bodyPosition =
            bodyWorld.getTranslation();

        if (!std::isfinite(bodyPosition.x) ||
    !std::isfinite(bodyPosition.y) ||
    !std::isfinite(bodyPosition.z))
{
    std::cerr
        << "INVALID BODY POSITION: "
        << def.boneName
        << " = "
        << bodyPosition.x << ", "
        << bodyPosition.y << ", "
        << bodyPosition.z
        << '\n';

    __debugbreak();
}

        const Quat bodyRotation =
            bodyWorld.getRotation();



const float qLenSq =
    bodyRotation.x * bodyRotation.x +
    bodyRotation.y * bodyRotation.y +
    bodyRotation.z * bodyRotation.z +
    bodyRotation.w * bodyRotation.w;

if (!std::isfinite(qLenSq) ||
    qLenSq < 0.99f ||
    qLenSq > 1.01f)
{
    std::cerr
        << "INVALID BODY ROTATION: "
        << def.boneName
        << " quat = "
        << bodyRotation.x << ", "
        << bodyRotation.y << ", "
        << bodyRotation.z << ", "
        << bodyRotation.w
        << " lenSq = "
        << qLenSq
        << '\n';

    __debugbreak();
}

        JPH::RefConst<JPH::Shape> shape =
            CreateShape(def);

        if (!shape)
        {
            std::cerr
                << "Ragdoll: unsupported shape for "
                << def.boneName
                << '\n';

            continue;
        }

        JPH::BodyCreationSettings settings(
            shape,
            ToJoltPosition(bodyPosition),
            ToJoltQuat(bodyRotation),
            JPH::EMotionType::Kinematic,
            Layers::MOVING
        );

        settings.mFriction =
            def.friction;

        settings.mRestitution =
            def.restitution;


        JPH::Body* body =
            bodyInterface.CreateBody(
                settings
            );

        if (!body)
        {
            std::cerr
                << "Ragdoll: failed to create body for "
                << def.boneName
                << '\n';

            continue;
        }


        const JPH::BodyID bodyId =
            body->GetID();

        bodyInterface.AddBody(
            bodyId,
            JPH::EActivation::Activate
        );

        const entt::entity bodyEntity =
            registry.create();

        RagdollBodyComponent ragdollBody;

        ragdollBody.bodyId =
            bodyId;

        ragdollBody.restMass =
            def.mass;

        registry.emplace<RagdollBodyComponent>(
            bodyEntity,
            ragdollBody
        );

        TransformComponent transform;

        transform.position =
            def.localOffset;

        transform.rotation =
            def.localRotationOffset;

        transform.scale =
            Vector3(
                1.0f,
                1.0f,
                1.0f
            );

        registry.emplace<TransformComponent>(
            bodyEntity,
            transform
        );

        BoneComponent boneComponent;

        boneComponent.boneIndex =
            boneId;

        registry.emplace<BoneComponent>(
            bodyEntity,
            boneComponent
        );

        HealthComponent health;

        health.current =
            def.joint.health;

        health.maximum =
            def.joint.health;

        registry.emplace<HealthComponent>(
            bodyEntity,
            health
        );

        JointComponent joint;

        joint.breakForceThreshold =
            def.joint.breakForceThreshold;

        joint.driveState =
            JointComponent::DriveState::KinematicFollow;

        registry.emplace<JointComponent>(
            bodyEntity,
            std::move(joint)
        );

        ParentComponent parent;

        parent.parent =
            socketEntity;

        registry.emplace<ParentComponent>(
            bodyEntity,
            parent
        );

        boneToBodyEntity[boneId] =
            bodyEntity;

        boneToBody[boneId] =
            bodyId;
    }

    for (const auto& [childBone, childBodyEntity] :
         boneToBodyEntity)
    {
        uint32_t parentBone =
            skeleton.bones[childBone].parentId;


        while (
            parentBone != INVALID_BONE &&
            !boneToBody.contains(parentBone)
        )
        {
            parentBone =
                skeleton.bones[parentBone].parentId;
        }


        JointComponent& joint =
            registry.get<JointComponent>(
                childBodyEntity
            );

        if (parentBone == INVALID_BONE)
        {
            // Root physical body -- no parent to break away from.
            continue;
        }


        const Mat4 jointWorld =
            GetBoneGlobal(
                skeleton,
                childBone
            );

        const Vector3 jointPosition =
            jointWorld.getTranslation();

        const Mat4& parentBodyWorld =
            boneToBodyWorld.at(parentBone);

        const Mat4& childBodyWorld =
            boneToBodyWorld.at(childBone);

        joint.parentAnchorLocal =
            parentBodyWorld
                .inverse()
                .multiplyVec(
                    jointPosition
                );

        joint.childAnchorLocal =
            childBodyWorld
                .inverse()
                .multiplyVec(
                    jointPosition
                );

        joint.parentBodyEntity =
            boneToBodyEntity.at(parentBone);
    }
}

} 