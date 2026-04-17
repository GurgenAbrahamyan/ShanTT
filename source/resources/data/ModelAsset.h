#pragma once

#include "../../math_custom/Mat4.h"
#include "../assets/Material.h"
#include "../assets/RenderMesh.h"

struct MeshEntry {
    RenderMesh* mesh;
    Material* material;
    Mat4 localTransform;
};

struct ModelAsset
{
    std::string name;
    std::string path;
    std::vector<MeshEntry> meshes;
};