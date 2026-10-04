#include "PhysicsSyncSystem.h"
#include "ecs/components/physics/PhysicsBodyComponent.h"
#include "ecs/components/core/TransformComponent.h"
#include "scene/SceneContext.h"

#include <Jolt/Physics/Body/BodyInterface.h>

void PhysicsSyncSystem::Update(SceneContext& context, float)
{
    JPH::BodyInterface& bodyInterface = context.engine.physics.GetSystem().GetBodyInterface();

    registry.view<PhysicsBodyComponent, TransformComponent>().each(
        [&](PhysicsBodyComponent& body, TransformComponent& transform)
        {
            JPH::RVec3 pos;
            JPH::Quat  rot;
            bodyInterface.GetPositionAndRotation(body.bodyId, pos, rot);

            transform.position = Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
            transform.rotation = Quat(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
            // scale is untouched — Jolt doesn't simulate scale, it stays whatever you authored it as
        }
    );
}