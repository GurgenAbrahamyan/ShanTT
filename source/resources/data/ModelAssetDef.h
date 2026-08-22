#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"
#include "../managers/MeshHandleTypes.h"   // MeshRef = variant<StaticMeshID, SkinnedMeshID>
#include "../managers/MaterialHandleTypes.h"
#include "../managers/SkeletonHandleTypes.h"

struct ModelPartDef {
    std::string name;

    MeshRef    mesh;
    MaterialID material;

    uint32_t parentPartIndex = UINT32_MAX;

    Vector3 localPosition{0.0f, 0.0f, 0.0f};
    Quat    localRotation;
    Vector3 localScale{1.0f, 1.0f, 1.0f};

    std::optional<uint32_t> attachBoneIndex;
};

struct ModelAssetDef {
    std::string name;
    std::string path;

    std::vector<ModelPartDef> parts;

    SkeletonID skeleton; 
};