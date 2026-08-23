#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "SkinnedVertex.h"

struct SkinnedMeshData {
    std::string name;
    std::vector<SkinnedVertex> vertices;
    std::vector<uint32_t> indices;
};