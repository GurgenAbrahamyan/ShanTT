#pragma once
#include <cstdint>

enum class PassId : uint32_t { Invalid = 0 };
enum class ResourceId : uint32_t { Invalid = 0 };

inline PassId nextPassId()
{
    static uint32_t counter = 1; 
    return static_cast<PassId>(counter++);
}