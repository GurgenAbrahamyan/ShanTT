#pragma once

#include "resources/assets/Texture.h"
#include "resources/managers/ModelManager.h"
#include "math_custom/Vector2.h"

struct EngineResources {
    ModelManager* modelManager{nullptr};
    Texture* brdfTexture{nullptr};

    int shadowSlots{0};
    Vector2 windowSize{};
};