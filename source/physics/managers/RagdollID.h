#pragma once
#include <cstdint>

struct RagdollAssetID
{
    uint32_t value = UINT32_MAX;

    bool valid() const
    {
        return value != UINT32_MAX;
    }
};