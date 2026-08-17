#pragma once

#include <cstdint>

enum class MaterialSlot : uint8_t {
    Albedo = 0,
    ARM,      
    Normal,
    Emissive,
    Height,
    Count
};