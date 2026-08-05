#pragma once
#include <memory>
#include "platform/IPlatform.h"
#include "EventBus.h"
#include "input/InputManager.h"
#include "scene/SceneManager.h"

class Renderer;
class PhysicsEngine;
class AssetManager;

#include "EngineContext.h"
#include "scene/SceneContext.h"
#include "render/data/EngineResources.h"  
#include "input/UiInput.h"

class Engine {
public:
    Engine();
    ~Engine();

    void run();
    void stop();
    long long getTimeMicro();

private:
    std::unique_ptr<IPlatform> platform;
    EventBus bus;
    InputManager input;

    UiInput debugUi;

    EngineResources rendererResources;   
    FrameRenderData frameData;          

    Renderer renderer;

    PhysicsEngine physicsEngine;
    AssetManager assetManager;

    EngineContext engineContext;
    SceneContext sceneContext;

    SceneManager sceneManager;

    bool running;
    float accumulator;
    static constexpr float PHYSICS_STEP = 1.0f / 60.0f;

    int framesThisSecond;
    float timeSinceLastFpsPrint;
};