#pragma once
#pragma once
#include "../../../resources/assets/CubeMap.h"

struct CubeMapComponent {
    CubeMap* cubeMap = nullptr;
    float intensity = 0.5f;
};