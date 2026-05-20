#pragma once
#pragma once
#include "../../../resources/assets/CubeMap.h"

struct CubeMapComponent {
    CubeMap* cubeMap = nullptr;
    float intensity = 1.0f;
    float dirLightInfluence = 0.0f;
};