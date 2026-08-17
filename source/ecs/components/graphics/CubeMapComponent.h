#pragma once
#pragma once
#include "resources/managers/EnvironmentMap.h"

struct CubeMapComponent
{
    EnvironmentMap environment;

    float intensity = 1.0f;
    float dirLightInfluence = 1.0f;
};