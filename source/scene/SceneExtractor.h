#pragma once

#include "EnTT/entt.hpp"
#include "render/data/FrameRenderData.h"

class SceneExtractor {
public:
    static void extract(entt::registry& registry, FrameRenderData& frameData);

private:
    static Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);
};