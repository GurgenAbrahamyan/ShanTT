#pragma once

#include "../../math_custom/Mat4.h"
#include "../managers/MeshHandleTypes.h"
#include "../managers/MaterialHandleTypes.h"

#include <string>
#include <vector>

struct MeshEntry {
    MeshID mesh;
    MaterialID material;
    Mat4 localTransform = Mat4();
};

struct ModelAsset {
    std::string name;
    std::string path;

    std::vector<MeshEntry> meshes;
};