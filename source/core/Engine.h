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
#include "input/UIInput.h"

class Engine {
public:
    Engine();
    ~Engine();

    void run();
    void stop();
    long long getTimeMicro();

    SceneContext& getSceneContext(){ return sceneContext; }

private:
    std::unique_ptr<IPlatform> platform;
    EventBus bus;
    InputManager input;

    AssetManager assetManager;
    
    UiInput debugUi;

    FrameRenderData frameData;          

    Renderer renderer;

    PhysicsEngine physicsEngine;

    EngineContext engineContext;
    SceneContext sceneContext;

    SceneManager sceneManager;

    bool running;
    float accumulator;
    static constexpr float PHYSICS_STEP = 1.0f / 60.0f;

    int framesThisSecond;
    float timeSinceLastFpsPrint;
};