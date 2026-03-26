#pragma once
#include <vector>
#include <string>
#include "../../math_custom/Mat4.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"

#include "RenderMesh.h"
#include "Material.h"

class Model {
public:
 
    struct SubMesh {
        int id;
        RenderMesh* mesh = nullptr;
        Material* material = nullptr;



    };

    Model() {

    

    }

    
    void addSubMesh(int id, RenderMesh* mesh, Material* material, Mat4 localTransform) {
        subMeshes.push_back({ id, mesh, material, localTransform});
    }

    const std::vector<SubMesh>& getSubMeshes() const { return subMeshes; }

 
    void setPosition(const Vector3& pos) { position = pos; }
    const Vector3& getPosition() const { return position; }

    void setOrientation(const Quat& orient) { orientation = orient; }
    const Quat& getOrientation() const { return orientation; }

    void setScale(const Vector3& s) { scale = s; }
    const Vector3& getScale() const { return scale; }

   
    Mat4 getGlobalTransform() const {
        return Mat4::translate(position) * Mat4::fromQuat(orientation) * Mat4::scale(scale);
    }

    int getID() const { return ID; }
	void setID(int id) { ID = id; }

private:
    std::vector<SubMesh> subMeshes;


    Vector3 position = Vector3(0, 0, 0);
    Quat orientation = Quat();
    Vector3 scale = Vector3(1, 1, 1);

    int ID = -1;
};
