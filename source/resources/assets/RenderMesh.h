#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "../../render/backend/containers/VAO.h"
#include "../../render/backend/containers/VBO.h"
#include "../../render/backend/containers/EBO.h"

#include "../data/StaticVertex.h"      
#include "../data/SkinnedVertex.h" 
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Mat4.h"

template <typename VertexT>
class RenderMesh {
public:
    RenderMesh(
        const std::vector<VertexT>& vertices,
        const std::vector<unsigned int>& indices
    );

    ~RenderMesh();

    RenderMesh(const RenderMesh&) = delete;
    RenderMesh& operator=(const RenderMesh&) = delete;
    RenderMesh(RenderMesh&&) = delete;
    RenderMesh& operator=(RenderMesh&&) = delete;

    void bind() const;
    int indexCount() const;

    void setupInstanceVBO(std::size_t instanceCount);
    VBO* getInstanceVBO() const;

    void setupSkinnedTransformVBO(std::size_t instanceCount);
    VBO* getSkinnedTransformVBO() const;

    void setupSkinnedPaletteOffsetVBO(std::size_t instanceCount);
    VBO* getSkinnedPaletteOffsetVBO() const;

private:
    void setupBuffers();
    void calculateTangents();

private:
    VAO* vao = nullptr;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    VBO* instanceVBO = nullptr;
    std::size_t instanceVBOCapacity = 0;

    VBO* skinnedTransformVBO = nullptr;
    std::size_t skinnedTransformVBOCapacity = 0;

    VBO* skinnedPaletteOffsetVBO = nullptr;
    std::size_t skinnedPaletteOffsetVBOCapacity = 0;

    std::vector<VertexT> vertices;
    std::vector<unsigned int> indices;
};

template <typename VertexT>
RenderMesh<VertexT>::RenderMesh(
    const std::vector<VertexT>& vertices,
    const std::vector<unsigned int>& indices
)
    : vertices(vertices),
      indices(indices)
{
    calculateTangents();
    setupBuffers();
}

template <typename VertexT>
RenderMesh<VertexT>::~RenderMesh()
{
    delete vao;
    delete vbo;
    delete ebo;
    delete instanceVBO;
    delete skinnedTransformVBO;
    delete skinnedPaletteOffsetVBO;
}

template <typename VertexT>
void RenderMesh<VertexT>::bind() const
{
    if (vao) {
        vao->Bind();
    }
}

template <typename VertexT>
int RenderMesh<VertexT>::indexCount() const
{
    return static_cast<int>(indices.size());
}

template <typename VertexT>
void RenderMesh<VertexT>::calculateTangents()
{
    // pos/uv/normal/tangent/tangentW exist on both StaticVertex and
    // SkinnedVertex, so this needs no branching.
    for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const unsigned int i0 = indices[i];
        const unsigned int i1 = indices[i + 1];
        const unsigned int i2 = indices[i + 2];

        Vector3 edge1 = vertices[i1].pos - vertices[i0].pos;
        Vector3 edge2 = vertices[i2].pos - vertices[i0].pos;

        Vector2 duv1 = vertices[i1].uv - vertices[i0].uv;
        Vector2 duv2 = vertices[i2].uv - vertices[i0].uv;

        float denom = duv1.x * duv2.y - duv2.x * duv1.y;

        if (std::fabs(denom) < 1e-6f) {
            continue;
        }

        float f = 1.0f / denom;

        Vector3 tangent;
        tangent.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
        tangent.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
        tangent.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);

        vertices[i0].tangent += tangent;
        vertices[i1].tangent += tangent;
        vertices[i2].tangent += tangent;
    }

    for (auto& vertex : vertices)
    {
        Vector3 normal = vertex.normal;
        Vector3 tangent = vertex.tangent;

        Vector3 orthoT = tangent - normal * normal.dot(tangent);

        if (orthoT.lengthSquared() > 1e-8f)
        {
            orthoT = orthoT.normalized();
        }
        else
        {
            orthoT = normal.orthogonal();
        }

        Vector3 bitangent = normal.cross(orthoT);
        Vector3 computedBitangent = normal.cross(tangent);

        vertex.tangentW =
            (bitangent.dot(computedBitangent) < 0.0f) ? -1.0f : 1.0f;

        vertex.tangent = orthoT;
    }
}

template <typename VertexT>
void RenderMesh<VertexT>::setupBuffers()
{
    if (vao) {
        return;
    }

    vao = new VAO();
    vao->Bind();

    vbo = new VBO(
        vertices.data(),
        vertices.size() * sizeof(VertexT),
        false
    );

    ebo = new EBO(
        indices.data(),
        indices.size() * sizeof(unsigned int),
        false
    );

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, pos));
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, uv));
    vao->LinkAttrib(*vbo, 3, 3, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, normal));
    vao->LinkAttrib(*vbo, 4, 3, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, tangent));
    vao->LinkAttrib(*vbo, 5, 1, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, tangentW));

    if constexpr (std::is_same_v<VertexT, SkinnedVertex>)
    {
        vao->LinkAttribI(*vbo, 10, 4, GL_UNSIGNED_INT, sizeof(VertexT), (void*)offsetof(VertexT, boneIds));
        vao->LinkAttrib(*vbo, 11, 4, GL_FLOAT, sizeof(VertexT), (void*)offsetof(VertexT, weights));
    }

    vao->Unbind();
}

template <typename VertexT>
void RenderMesh<VertexT>::setupInstanceVBO(
    std::size_t instanceCount)
{
    if (instanceCount <= instanceVBOCapacity)
        return;

    if (!vao)
        setupBuffers();

    const std::size_t newCapacity =
        instanceCount + instanceCount / 2;

    if (!instanceVBO)
    {
        instanceVBO = new VBO(
            nullptr,
            newCapacity * sizeof(Mat4),
            true
        );
    }
    else
    {
        instanceVBO->Bind();

        glBufferData(
            GL_ARRAY_BUFFER,
            newCapacity * sizeof(Mat4),
            nullptr,
            GL_DYNAMIC_DRAW
        );
    }

    vao->Bind();

    instanceVBO->Bind();

    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(6 + i);

        glVertexAttribPointer(
            6 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Mat4),
            reinterpret_cast<void*>(
                sizeof(float) * 4 * i
            )
        );

        glVertexAttribDivisor(
            6 + i,
            1
        );
    }

    vao->Unbind();

    instanceVBOCapacity =
        newCapacity;
}

template <typename VertexT>
void RenderMesh<VertexT>::setupSkinnedTransformVBO(
    std::size_t instanceCount)
{
    static_assert(
        std::is_same_v<VertexT, SkinnedVertex>,
        "setupSkinnedTransformVBO() requires SkinnedVertex"
    );

    if (instanceCount <= skinnedTransformVBOCapacity)
        return;

    if (!vao)
        setupBuffers();

    const std::size_t newCapacity =
        instanceCount + instanceCount / 2;

    if (!skinnedTransformVBO)
    {
        skinnedTransformVBO = new VBO(
            nullptr,
            newCapacity * sizeof(Mat4),
            true
        );
    }
    else
    {
        skinnedTransformVBO->Bind();

        glBufferData(
            GL_ARRAY_BUFFER,
            newCapacity * sizeof(Mat4),
            nullptr,
            GL_DYNAMIC_DRAW
        );
    }

    vao->Bind();
    skinnedTransformVBO->Bind();

    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(6 + i);

        glVertexAttribPointer(
            6 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Mat4),
            reinterpret_cast<void*>(sizeof(float) * 4 * i)
        );

        glVertexAttribDivisor(6 + i, 1);
    }

    vao->Unbind();

    skinnedTransformVBOCapacity = newCapacity;
}

template <typename VertexT>
void RenderMesh<VertexT>::setupSkinnedPaletteOffsetVBO(
    std::size_t instanceCount)
{
    static_assert(
        std::is_same_v<VertexT, SkinnedVertex>,
        "setupSkinnedPaletteOffsetVBO() requires SkinnedVertex"
    );

    if (instanceCount <= skinnedPaletteOffsetVBOCapacity)
        return;

    if (!vao)
        setupBuffers();

    const std::size_t newCapacity =
        instanceCount + instanceCount / 2;

    if (!skinnedPaletteOffsetVBO)
    {
        skinnedPaletteOffsetVBO = new VBO(
            nullptr,
            newCapacity * sizeof(uint32_t),
            true
        );
    }
    else
    {
        skinnedPaletteOffsetVBO->Bind();

        glBufferData(
            GL_ARRAY_BUFFER,
            newCapacity * sizeof(uint32_t),
            nullptr,
            GL_DYNAMIC_DRAW
        );
    }

    vao->Bind();
    skinnedPaletteOffsetVBO->Bind();

    // Integer attribute — glVertexAttribIPointer, not glVertexAttribPointer.
    glEnableVertexAttribArray(12);

    glVertexAttribIPointer(
        12,
        1,
        GL_UNSIGNED_INT,
        sizeof(uint32_t),
        reinterpret_cast<void*>(0)
    );

    glVertexAttribDivisor(12, 1);

    vao->Unbind();

    skinnedPaletteOffsetVBOCapacity = newCapacity;
}

template <typename VertexT>
VBO* RenderMesh<VertexT>::getInstanceVBO() const
{
    return instanceVBO;
}


template <typename VertexT>
VBO* RenderMesh<VertexT>::getSkinnedTransformVBO() const
{
    return skinnedTransformVBO;
}

template <typename VertexT>
VBO* RenderMesh<VertexT>::getSkinnedPaletteOffsetVBO() const
{
    return skinnedPaletteOffsetVBO;
}


using StaticMesh  = RenderMesh<StaticVertex>;
using SkinnedMesh = RenderMesh<SkinnedVertex>;