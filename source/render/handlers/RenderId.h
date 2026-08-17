#pragma once
#include <cstdint>

enum class PassId : uint32_t { Invalid = 0 };
enum class ResourceId : uint32_t { Invalid = 0 };

constexpr PassId INVALID_PASS_ID = PassId::Invalid;
constexpr ResourceId INVALID_RESOURCE_ID = ResourceId::Invalid;

inline PassId nextPassId()
{
    static uint32_t counter = 1;
    return static_cast<PassId>(counter++);
}

inline ResourceId nextResourceId()
{
    static uint32_t counter = 1;
    return static_cast<ResourceId>(counter++);
}