#pragma once
#include "TextureHandleTypes.h"

struct EnvironmentMap {
    CubeMapID env;
    CubeMapID irradiance;
    CubeMapID prefiltered;
};