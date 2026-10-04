#pragma once

#include <entt/entt.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/Constraint.h>

#include "math_custom/Vector3.h"

struct JointComponent
{
    enum class DriveState
    {
        KinematicFollow,
        PoweredDynamic,
        FreeDynamic
    };

    JPH::Ref<JPH::Constraint> constraint;

    DriveState driveState = DriveState::PoweredDynamic;

    float breakForceThreshold = 4000.0f;


    entt::entity parentBodyEntity = entt::null;

    Vector3 parentAnchorLocal = Vector3(0, 0, 0);
    Vector3 childAnchorLocal  = Vector3(0, 0, 0);

    bool IsMotorActive() const
    {
        return driveState == DriveState::PoweredDynamic;
    }
};