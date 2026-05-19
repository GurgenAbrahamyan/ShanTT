#pragma once

#include "../../math_custom/Mat4.h"
#include "../assets/Material.h"
#include "../assets/RenderMesh.h"

struct MeshEntry {
    RenderMesh* mesh = nullptr;
    Material* material = nullptr;
    Mat4 localTransform = Mat4();
};

struct ModelAsset
{
    std::string name;
    std::string path;
    std::vector<MeshEntry> meshes;
};