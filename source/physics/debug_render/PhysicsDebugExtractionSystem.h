#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyManager.h>

#include "physics/PhysicsEngine.h"
#include "physics/debug_render/DebugRenderData.h"
#include "physics/debug_render/DebugRender.h"

#include "scene/IExtractionSystem.h"
#include "render/data/FrameRenderData.h"

#include "ecs/components/graphics/ActiveCameraTag.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/core/TransformComponent.h"


class PhysicsDebugExtractionSystem
    : public IExtractionSystem
{
public:

    explicit PhysicsDebugExtractionSystem(
        PhysicsEngine& physics
    )
        : m_physics(physics)
        , m_debugRenderer(physics.getRenderer())
    {
    }


    void extract(
        entt::registry& registry,
        FrameRenderData& out
    ) override
    {
        auto& debugData =
            out.Emplace<DebugRenderPhysData>();


        m_debugRenderer.beginFrame(
            debugData
        );


        Vector3 cameraPosition =
            getCameraPosition(registry);


        m_debugRenderer.setCameraPosition(
            JPH::RVec3(
                cameraPosition.x,
                cameraPosition.y,
                cameraPosition.z
            )
        );


        JPH::BodyManager::DrawSettings settings;

        settings.mDrawShape = true;
        settings.mDrawShapeWireframe = true;
        settings.mDrawBoundingBox = false;
        settings.mDrawCenterOfMassTransform = true;
        settings.mDrawWorldTransform = true;


        m_physics.GetSystem().DrawBodies(
            settings,
            &m_debugRenderer
        );
    }


private:

    Vector3 getCameraPosition(
        entt::registry& registry
    )
    {
        auto view =
            registry.view<
                ActiveCameraTag,
                CameraComponent,
                TransformComponent
            >();


        for (auto entity : view)
        {
            const auto& transform =
                view.get<TransformComponent>(entity);

            return transform.position;
        }


        return Vector3(
            0.0f,
            0.0f,
            0.0f
        );
    }


private:

    PhysicsEngine& m_physics;

    JoltDebugRenderer& m_debugRenderer;
};