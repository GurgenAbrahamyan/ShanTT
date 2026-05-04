#pragma once
#include <vector>
#include "../../render/backend/containers/VAO.h"
#include "../../render/backend/containers/VBO.h"
#include "../../render/backend/containers/EBO.h"
#include "../data/Vertex.h"
#include "RenderMesh.h"
#include "../../math_custom/Vector3.h"

class Mat4;

class StaticMesh : public RenderMesh {
public:
    StaticMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~StaticMesh() override;
    
    void bind() override;
    void setupBuffers() override;
    int indexCount() const override;
    void setColor(const Vector3& col) override;
   
    void setupInstanceVBO(size_t maxInstances) override;
	void calculateTangents();
    GLuint getInstanceVBO() override;
    
private:
 
    
    VAO* VAO1 = nullptr;
    VBO* VBO1 = nullptr;
    EBO* EBO1 = nullptr;
    
    GLuint instanceVBO = 0;
    size_t instanceVBOCapacity = 0;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};