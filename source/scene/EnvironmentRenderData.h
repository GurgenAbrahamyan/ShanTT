#pragma once

#include "resources/assets/CubeMap.h"

#pragma once

#include "resources/assets/CubeMap.h"

struct EnvironmentRenderData
{
    const CubeMap* prefilter = nullptr;
    const CubeMap* env = nullptr;
    const CubeMap* irr = nullptr;

    float intensity = 1.0f;
    float dirLightInfluence = 1.0f;
};