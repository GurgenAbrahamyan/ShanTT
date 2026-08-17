#pragma once
#include "../components/physics/RigidBodyComponent.h"
#include "../components/physics/CollisionShapeComponent.h"
#include "../components/core/TransformComponent.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"
#include "EnTT/entt.hpp"
#include <iostream>
namespace PhysicsComponentFactory {

    TransformComponent createTransform(Vector3 translation = Vector3(), Quat rotation = Quat(), Vector3 scale = Vector3(1, 1, 1)) {
        TransformComponent transform;
        transform.position = translation;
        transform.rotation = rotation;
        transform.scale = scale;
        return transform;
    }

    RigidBodyComponent createRigidBody(entt::registry& registry, entt::entity entity, Vector3 translation = Vector3(), Quat rotation = Quat(), Vector3 scale = Vector3(1, 1, 1), float mass = 0 ) {
        RigidBodyComponent rb;
        rb.mass = mass;
        rb.invmass = mass > 0.0f ? 1.0f / mass : 0.0f;
        rb.linearVelocity = Vector3(0, 0, 0);
        rb.angularVelocity = Vector3(0, 0, 0);
        rb.forceAccum = Vector3(0, 0, 0);

        if (!registry.all_of<TransformComponent>(entity)) {
            std::cout << "creating transform";
            registry.emplace<TransformComponent>(entity, createTransform(translation, rotation, scale));
        }

        return rb;
    }

    



    RigidBodyComponent createStaticBody(entt::registry& reg, entt::entity entity) {
        return createRigidBody(reg, entity);
    }

    CollisionShapeComponent createCubeShape(
        Vector3 scale,
        Vector3 localPosition = Vector3(0, 0, 0),
        Quat    localRotation = Quat(),
        Vector3 localScale = Vector3(1, 1, 1))
    {
        float hw = scale.x * 0.5f;
        float hd = scale.y * 0.5f;
        float hh = scale.z * 0.5f;

        CollisionShapeComponent shape;
        shape.vertices = {
            Vector3(-hw,-hd,-hh), Vector3(hw,-hd,-hh),
            Vector3(hw, hd,-hh), Vector3(-hw, hd,-hh),
            Vector3(-hw,-hd, hh), Vector3(hw,-hd, hh),
            Vector3(hw, hd, hh), Vector3(-hw, hd, hh)
        };
        shape.indices = {
            0,1,2, 2,3,0,
            4,6,5, 6,4,7,
            0,1,5, 5,4,0,
            2,3,7, 7,6,2,
            0,3,7, 7,4,0,
            1,2,6, 6,5,1
        };
        shape.localPosition = localPosition;
        shape.localRotation = localRotation;
        shape.localScale = localScale;
        return shape;
    }

    CollisionShapeComponent createCustomShape(
        std::vector<Vector3>      vertices,
        std::vector<unsigned int> indices,
        Vector3 localPosition = Vector3(0, 0, 0),
        Quat    localRotation = Quat(),
        Vector3 localScale = Vector3(1, 1, 1))
    {
        CollisionShapeComponent shape;
        shape.vertices = vertices;
        shape.indices = indices;
        shape.localPosition = localPosition;
        shape.localRotation = localRotation;
        shape.localScale = localScale;
        return shape;
    }
}