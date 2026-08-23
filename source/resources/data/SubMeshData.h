#pragma once
#include <string>
#include <cstdint>
#include <optional>

#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"

enum class MeshKind : uint8_t {
    Static,
    Skinned
};

struct SubMeshData {
    std::string name;

    MeshKind meshKind = MeshKind::Static;
    uint32_t meshIndex     = UINT32_MAX;
    uint32_t materialIndex = UINT32_MAX;

    uint32_t parentSubmeshIndex = UINT32_MAX; 

    Vector3 localPosition{0.0f, 0.0f, 0.0f};
    Quat    localRotation;
    Vector3 localScale{1.0f, 1.0f, 1.0f};

    std::optional<uint32_t> attachBoneIndex;
};