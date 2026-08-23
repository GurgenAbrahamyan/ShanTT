#pragma once
#include <string>
#include <vector>
#include <optional>

#include "StaticMeshData.h"
#include "SkinnedMeshData.h"
#include "MaterialData.h"
#include "SubMeshData.h"
#include "SkeletonData.h"

struct ModelData {
    std::string modelName;

    std::vector<StaticMeshData>  staticMeshes;
    std::vector<SkinnedMeshData> skinnedMeshes;
    std::vector<MaterialData>    materials;
    std::vector<SubMeshData>     submeshes;

    std::optional<SkeletonData> skeleton;
};