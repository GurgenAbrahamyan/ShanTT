#pragma once
#include <vector>
#include "AnimationHandleTypes.h"

struct AnimationState
{
    AnimationID clip;
    float time = 0.0f;
    float speed = 1.0f;
    bool looping = true;
    bool playing = true;

    uint32_t lastSeenClipVersion = 0;                 
    std::vector<std::vector<size_t>> cacheIndices;     
};