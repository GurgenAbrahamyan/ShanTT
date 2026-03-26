#pragma once
#include <vector>
#include "../../../resources/assets/Material.h"
#include "../../../resources/assets/RenderMesh.h"
#include "../../../math_custom/Mat4.h"


struct MeshEntry {
    RenderMesh* mesh;          
    Material* material;        
    Mat4 localTransform;        
};

struct ModelComponent {
   

    std::vector<MeshEntry> meshes; 
 };
