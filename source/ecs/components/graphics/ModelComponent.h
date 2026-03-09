#pragma once
#include <vector>
#include "../../../resources/assets/Material.h"
#include "../../../resources/assets/RenderMesh.h"
#include "../../../math_custom/Mat4.h"


struct MeshEntry {
    RenderMesh* mesh;           // pointer to the mesh
    Material* material;         // pointer to material
    Mat4 localTransform;        // transform relative to model origin
};

struct ModelComponent {
   

    std::vector<MeshEntry> meshes; // all submeshes of this model
 };
