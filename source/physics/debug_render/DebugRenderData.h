#pragma once

#include <vector>

#include "math_custom/Vector3.h"
#include "math_custom/Vector4.h"


struct DebugVertex
{
    Vector3 position;
    Vector4 color;
};


struct DebugRenderPhysData
{
    std::vector<DebugVertex> lines;
    std::vector<DebugVertex> triangles;


    void clear()
    {
        lines.clear();
        triangles.clear();
    }


    void addLine(
        const Vector3& from,
        const Vector3& to,
        const Vector4& color
    )
    {
        lines.push_back({
            from,
            color
        });

        lines.push_back({
            to,
            color
        });
    }


    void addTriangle(
        const Vector3& a,
        const Vector3& b,
        const Vector3& c,
        const Vector4& color
    )
    {
        triangles.push_back({
            a,
            color
        });

        triangles.push_back({
            b,
            color
        });

        triangles.push_back({
            c,
            color
        });
    }
};