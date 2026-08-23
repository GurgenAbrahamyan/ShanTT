#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "StaticVertex.h"

struct StaticMeshData {
    std::string name;
    std::vector<StaticVertex> vertices;
    std::vector<uint32_t> indices;
};