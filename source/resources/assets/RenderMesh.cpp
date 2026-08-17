#include "RenderMesh.h"

#include <cmath>
#include <cstddef>

RenderMesh::RenderMesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices
)
    : vertices(vertices),
      indices(indices)
{
    calculateTangents();
    setupBuffers();
}

RenderMesh::~RenderMesh()
{
    delete vao;
    delete vbo;
    delete ebo;

    if (instanceVBO != 0) {
        glDeleteBuffers(1, &instanceVBO);
    }
}

void RenderMesh::bind() const
{
    if (vao) {
        vao->Bind();
    }
}

int RenderMesh::indexCount() const
{
    return static_cast<int>(indices.size());
}

void RenderMesh::calculateTangents()
{
    for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const unsigned int i0 = indices[i];
        const unsigned int i1 = indices[i + 1];
        const unsigned int i2 = indices[i + 2];

        Vector3 edge1 =
            vertices[i1].position - vertices[i0].position;

        Vector3 edge2 =
            vertices[i2].position - vertices[i0].position;

        Vector2 duv1 =
            vertices[i1].uv - vertices[i0].uv;

        Vector2 duv2 =
            vertices[i2].uv - vertices[i0].uv;

        float denom =
            duv1.x * duv2.y -
            duv2.x * duv1.y;

        if (std::fabs(denom) < 1e-6f) {
            continue;
        }

        float f = 1.0f / denom;

        Vector3 tangent;

        tangent.x =
            f * (duv2.y * edge1.x -
                 duv1.y * edge2.x);

        tangent.y =
            f * (duv2.y * edge1.y -
                 duv1.y * edge2.y);

        tangent.z =
            f * (duv2.y * edge1.z -
                 duv1.y * edge2.z);

        vertices[i0].tangent += tangent;
        vertices[i1].tangent += tangent;
        vertices[i2].tangent += tangent;
    }

    for (auto& vertex : vertices)
    {
        Vector3 normal = vertex.normal;
        Vector3 tangent = vertex.tangent;

        // Gram-Schmidt orthogonalization.
        Vector3 orthoT =
            tangent - normal * normal.dot(tangent);

        if (orthoT.lengthSquared() > 1e-8f)
        {
            orthoT = orthoT.normalized();
        }
        else
        {
            orthoT = normal.orthogonal();
        }

        // Determine tangent handedness.
        Vector3 bitangent = normal.cross(orthoT);
        Vector3 computedBitangent = normal.cross(tangent);

        vertex.tangentW =
            (bitangent.dot(computedBitangent) < 0.0f)
                ? -1.0f
                : 1.0f;

        vertex.tangent = orthoT;
    }
}

void RenderMesh::setupBuffers()
{
    if (vao) {
        return;
    }

    vao = new VAO();
    vao->Bind();

    vbo = new VBO(
        vertices.data(),
        vertices.size() * sizeof(Vertex),
        false
    );

    ebo = new EBO(
        indices.data(),
        indices.size() * sizeof(unsigned int),
        false
    );

    vao->LinkAttrib(
        *vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    vao->LinkAttrib(
        *vbo,
        2,
        2,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, uv)
    );

    vao->LinkAttrib(
        *vbo,
        3,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );

    vao->LinkAttrib(
        *vbo,
        4,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, tangent)
    );

    vao->LinkAttrib(
        *vbo,
        5,
        1,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, tangentW)
    );

    vao->Unbind();
}

void RenderMesh::setupInstanceVBO(std::size_t instanceCount)
{
    if (instanceCount <= instanceVBOCapacity) {
        return;
    }

    if (!vao) {
        setupBuffers();
    }

    const std::size_t newCapacity =
        instanceCount + instanceCount / 2;

    if (instanceVBO == 0) {
        glGenBuffers(1, &instanceVBO);
    }

    vao->Bind();

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        newCapacity * sizeof(Mat4),
        nullptr,
        GL_DYNAMIC_DRAW
    );

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

        glVertexAttribDivisor(6 + i, 1);
    }

    vao->Unbind();

    instanceVBOCapacity = newCapacity;
}

GLuint RenderMesh::getInstanceVBO() const
{
    return instanceVBO;
}