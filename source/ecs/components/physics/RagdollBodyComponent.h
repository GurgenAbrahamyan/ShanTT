#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

// ecs/components/physics/RagdollBodyComponent.h
#pragma once
#include <Jolt/Physics/Body/BodyID.h>
#include <string>
#include "physics/data/RagdollData.h"

struct RagdollBodyComponent
{
    JPH::BodyID bodyId;

    std::string     boneName;
    RagdollShapeType shapeType = RagdollShapeType::Capsule;

    float radius     = 0.05f;
    float halfHeight  = 0.1f;      // capsule
    Vector3 halfExtent = Vector3(0.1f, 0.1f, 0.1f); // box

    Vector3 localOffset         = Vector3(0, 0, 0);
    Quat    localRotationOffset = Quat();

    float restMass {};
};