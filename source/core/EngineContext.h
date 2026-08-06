#pragma once

#include "input/InputManager.h"
#include "core/EventBus.h"
#include "render/Renderer.h"
#include "physics/PhysicsEngine.h"
#include "resources/managers/AssetManager.h"
#include "platform/IPlatform.h"
struct EngineContext
{
    IPlatform& platform;

    float deltaTime = 0.0f;

    float totalTime = 0.0f;

    EventBus& events;

    Renderer& renderer;

    PhysicsEngine& physics;

    AssetManager& assets;

    InputManager& input;
};