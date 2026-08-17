#pragma once

#include <cstddef>
#include <vector>

#include "../../render/backend/containers/VAO.h"
#include "../../render/backend/containers/VBO.h"
#include "../../render/backend/containers/EBO.h"

#include "../data/Vertex.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Mat4.h"

class RenderMesh {
public:
    RenderMesh(
        const std::vector<Vertex>& vertices,
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
    GLuint getInstanceVBO() const;

private:
    void setupBuffers();
    void calculateTangents();

private:
    VAO* vao = nullptr;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    GLuint instanceVBO = 0;
    std::size_t instanceVBOCapacity = 0;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};