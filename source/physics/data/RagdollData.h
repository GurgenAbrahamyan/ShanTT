#pragma once

#include <string>
#include <vector>
#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"
enum class RagdollShapeType
{
    Capsule,
    Box,
    Sphere
};

struct RagdollShape
{
    RagdollShapeType type;

    float radius = 0.0f;
    float halfHeight = 0.0f;

    Vector3 halfExtent{};

};



struct RagdollJointDef                     
{
    float motorFrequency = 6.0f;
    float motorDamping = 1.0f;
    float maxMotorTorque = 800.0f;
    float breakForceThreshold = 4000.0f;
    float health = 100.0f;
};

struct RagdollBodyDef
{
    std::string boneName;

    RagdollShape shape;

    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;

    Vector3 localOffset{};                
    Quat localRotationOffset{};

    RagdollJointDef joint{};
};

struct RagdollAsset
{
    std::string name;

    std::vector<RagdollBodyDef> bodies;
};