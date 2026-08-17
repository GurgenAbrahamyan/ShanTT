#pragma once
#include <vector>
#include "render/data/ShadowData.h"

struct ShadowCasterComponent {
    std::vector<ShadowData> shadowData;
};