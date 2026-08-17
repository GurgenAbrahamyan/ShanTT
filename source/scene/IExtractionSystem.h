#pragma once

#include <entt/entt.hpp>
#include "render/data/FrameRenderData.h"

class IExtractionSystem
{
public:
    virtual ~IExtractionSystem() = default;


    virtual void onInit(entt::registry&) {}

    virtual void extract(entt::registry& registry, FrameRenderData& out) = 0;

    virtual void onShutdown() {}
};