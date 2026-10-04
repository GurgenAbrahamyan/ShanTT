#pragma once
#include "../components/physics/RigidBodyComponent.h"
#include "../components/physics/CollisionShapeComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/physics/RagDollComponent.h"

#include "resources/assets/Skeleton/Skeleton.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"
#include "EnTT/entt.hpp"
#include <iostream>
#include "physics/data/RagdollData.h"

#include "scene/SceneContext.h"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "ecs/factories/ModelSpawner.h"
#pragma once

#include "../components/physics/RigidBodyComponent.h"
#include "../components/physics/CollisionShapeComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/physics/RagDollComponent.h"

#include "resources/assets/Skeleton/Skeleton.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"

#include <EnTT/entt.hpp>

#include "physics/data/RagdollData.h"
#include "scene/SceneContext.h"
#include "ecs/factories/ModelSpawner.h"

namespace PhysicsComponentFactory
{
    TransformComponent createTransform(
        Vector3 translation = Vector3(),
        Quat rotation = Quat(),
        Vector3 scale = Vector3(1, 1, 1)
    );

    RigidBodyComponent createRigidBody(
        entt::registry& registry,
        entt::entity entity,
        Vector3 translation = Vector3(),
        Quat rotation = Quat(),
        Vector3 scale = Vector3(1, 1, 1),
        float mass = 0.0f
    );

    RigidBodyComponent createStaticBody(
        entt::registry& registry,
        entt::entity entity
    );

    CollisionShapeComponent createCubeShape(
        Vector3 scale,
        Vector3 localPosition = Vector3(0, 0, 0),
        Quat localRotation = Quat(),
        Vector3 localScale = Vector3(1, 1, 1)
    );

    CollisionShapeComponent createCustomShape(
        std::vector<Vector3> vertices,
        std::vector<unsigned int> indices,
        Vector3 localPosition = Vector3(0, 0, 0),
        Quat localRotation = Quat(),
        Vector3 localScale = Vector3(1, 1, 1)
    );

    void BuildRagdoll(
        entt::registry& registry,
        const SpawnedModel& model,
        const RagdollAsset& asset,
        Skeleton& skeleton,
        SceneContext& context
    );
}