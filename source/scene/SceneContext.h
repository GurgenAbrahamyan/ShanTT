#pragma once
#include <entt/entt.hpp>
#include "core/EngineContext.h"
struct SceneContext {
    public:
    EngineContext& engine;
    entt::registry& registry;
};