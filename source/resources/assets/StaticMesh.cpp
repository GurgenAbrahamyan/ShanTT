#include "StaticMesh.h"
#include <iostream>
#include "../../math_custom/Mat4.h"

StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& ind)
    : vertices(vertices), indices(ind), instanceVBO(0), instanceVBOCapacity(0)
{
	calculateTangents();
}

StaticMesh::~StaticMesh() {
    delete VAO1;
    delete VBO1;
    delete EBO1;

    if (instanceVBO != 0) {
        glDeleteBuffers(1, &instanceVBO);
    }
}

void StaticMesh::bind() {
    if (VAO1) {
        VAO1->Bind();
    }
}

int StaticMesh::indexCount() const {
    return static_cast<int>(indices.size());
}

void StaticMesh::calculateTangents()
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];

        Vector3 edge1 = vertices[i1].position - vertices[i0].position;
        Vector3 edge2 = vertices[i2].position - vertices[i0].position;
        Vector2 duv1 = vertices[i1].uv - vertices[i0].uv;
        Vector2 duv2 = vertices[i2].uv - vertices[i0].uv;

        float denom = duv1.x * duv2.y - duv2.x * duv1.y;
        if (fabs(denom) < 1e-6f) continue; 
        float f = 1.0f / denom;

        Vector3 tangent;
        tangent.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
        tangent.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
        tangent.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);

        vertices[i0].tangent += tangent;
        vertices[i1].tangent += tangent;
        vertices[i2].tangent += tangent;
    }

    for (auto& v : vertices)
    {
        Vector3 N = v.normal;
        Vector3 T = v.tangent;

        // Gram-Schmidt orthogonalize
        Vector3 orthoT = (T - N * N.dot(T)).normalized();

        // Handedness
        Vector3 B = N.cross(orthoT);
        Vector3 computedB = N.cross(T);
        v.tangentW = (B.dot(computedB) < 0.0f) ? -1.0f : 1.0f;

        v.tangent = orthoT;
    }
}


void StaticMesh::setupBuffers() {
    if (VAO1) return; // Already initialized

    std::cout << "Setting up buffers\n";

    VAO1 = new VAO();
    VAO1->Bind();

    VBO1 = new VBO(vertices.data(), vertices.size() * sizeof(Vertex), false);
    EBO1 = new EBO(indices.data(), indices.size() * sizeof(unsigned int), false);

    // Setup vertex attributes
    VAO1->LinkAttrib(*VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);                              // position
    VAO1->LinkAttrib(*VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));        // color
    VAO1->LinkAttrib(*VBO1, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, uv));           // uv
    VAO1->LinkAttrib(*VBO1, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));       // normal
	VAO1->LinkAttrib(*VBO1, 4, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tangent));     // tangent
	VAO1->LinkAttrib(*VBO1, 5, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tangentW));    // tangent handedness
    VAO1->Unbind();
}

void StaticMesh::setupInstanceVBO(size_t instanceCount) {
    // Only reallocate if we need more space
    if (instanceCount <= instanceVBOCapacity) {
        return;
    }

    // Make sure VAO is set up first
    if (!VAO1) {
        setupBuffers();
    }


    size_t newCapacity = instanceCount + (instanceCount / 2);

    if (instanceVBO == 0) {
        glGenBuffers(1, &instanceVBO);
    }

    VAO1->Bind();

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        newCapacity * sizeof(Mat4),
        nullptr,
        GL_DYNAMIC_DRAW
    );



    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(6 + i);
        glVertexAttribPointer(
            6 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Mat4),
            (void*)(sizeof(float) * 4 * i)
        );
        glVertexAttribDivisor(6 + i, 1);
    }

    VAO1->Unbind();

    instanceVBOCapacity = newCapacity;
}

GLuint StaticMesh::getInstanceVBO() {
    return instanceVBO;
}

void StaticMesh::setColor(const Vector3& col) {
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].color.setX(col.getX());
        vertices[i].color.setY(col.getY());
        vertices[i].color.setZ(col.getZ());
    }

    // Update VBO if already initialized
    if (VBO1) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO1->ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
}