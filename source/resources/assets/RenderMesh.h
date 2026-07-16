#pragma once

#include "../../math_custom/Vector3.h"
#include <vector>

#include "../../math_custom/Mat4.h"

#include "glad/glad.h"

class RenderMesh {
public:
    virtual ~RenderMesh() = default;

    virtual void bind() = 0;

    virtual void setupBuffers() = 0;
    virtual int indexCount() const = 0;

    virtual void setColor(const Vector3& col) = 0;

    //virtual Vertex& getVertex(int i) { return Vertex(); };
   
   // std::vector<Vertex>& getVertexes() { return []{} };
   
    

    void setID(int i) {
        ID = i;
    }
    int getID() {
       return ID;
    }

    virtual void setupInstanceVBO([[maybe_unused]]size_t maxInstances) { /*TO:DO*/};
    virtual void updateInstanceVBO([[maybe_unused]] const std::vector<Mat4>& matrices) {/*TO:DO*/};
    virtual GLuint getInstanceVBO() { return 0; };
    



protected:
  

   
    int ID = -1;

   
};