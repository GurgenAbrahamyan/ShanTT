#pragma once
#include "../core/EventBus.h"
#include <entt/entt.hpp>
struct SceneContext
{
    EventBus& events;
    entt::registry& registry;
};