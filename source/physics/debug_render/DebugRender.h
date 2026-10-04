#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "DebugRenderData.h"


class JoltDebugRenderer final
    : public JPH::DebugRendererSimple
{
public:
    JoltDebugRenderer()
    {
        Initialize();
    }

    void beginFrame(DebugRenderPhysData& data)
    {
        m_data = &data;
        m_data->clear();
    }

    void setCameraPosition(JPH::RVec3Arg position)
    {
        SetCameraPos(position);
    }

    void DrawLine(
        JPH::RVec3Arg inFrom,
        JPH::RVec3Arg inTo,
        JPH::ColorArg inColor
    ) override
    {
        if (!m_data)
            return;

        m_data->addLine(
            toVector3(inFrom),
            toVector3(inTo),
            toVector4(inColor)
        );
    }

    void DrawTriangle(
        JPH::RVec3Arg inV1,
        JPH::RVec3Arg inV2,
        JPH::RVec3Arg inV3,
        JPH::ColorArg inColor,
        ECastShadow
    ) override
    {
        if (!m_data)
            return;

        m_data->addTriangle(
            toVector3(inV1),
            toVector3(inV2),
            toVector3(inV3),
            toVector4(inColor)
        );
    }

    void DrawText3D(
        JPH::RVec3Arg,
        const JPH::string_view&,
        JPH::ColorArg,
        float
    ) override
    {
    }

private:
    static Vector3 toVector3(JPH::RVec3Arg value)
    {
        return Vector3(
            static_cast<float>(value.GetX()),
            static_cast<float>(value.GetY()),
            static_cast<float>(value.GetZ())
        );
    }

    static Vector4 toVector4(JPH::ColorArg value)
    {
        return Vector4(
            value.r / 255.0f,
            value.g / 255.0f,
            value.b / 255.0f,
            value.a / 255.0f
        );
    }

private:
    DebugRenderPhysData* m_data = nullptr;
};