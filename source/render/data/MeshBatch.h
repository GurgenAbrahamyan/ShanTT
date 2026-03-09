#pragma once
#include "../../resources/assets/RenderMesh.h"
struct MeshBatch {

    std::vector<Mat4> instances;

    MeshBatch() {
        instances.reserve(128);
    }
};
