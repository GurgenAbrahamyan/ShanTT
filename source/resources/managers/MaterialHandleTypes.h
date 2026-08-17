#pragma once

#include "ResourcePool.h"
#include <functional>

struct MaterialTag {};
using MaterialID = Handle<MaterialTag>;

struct MaterialIDHash
{
    std::size_t operator()(const MaterialID& id, const MaterialID other = {}) const noexcept
    {
        (void)other;
        std::size_t seed = std::hash<uint32_t>{}(id.index);

        seed ^= std::hash<uint32_t>{}(id.generation)
              + 0x9e3779b9
              + (seed << 6)
              + (seed >> 2);

        return seed;
    }
};