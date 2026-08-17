#pragma once

#include "scene/Scene.h"
#include "platform/GLFW/PlatformGLFW.h"

struct TestContext
{
    EventBus eventBus;
    Renderer renderer;
    PhysicsEngine physics;
    AssetManager assets;
    InputManager input{eventBus};
    PlatformGLFW platform;

    entt::registry registry;

    EngineContext& CreateEngineContext()
    {   
        static EngineContext ctx{
            platform,
            0.0f,
            0.0f,
            eventBus,
            renderer,
            physics,
            assets,
            input,
            nullptr
        };
        return ctx;
    }

    SceneContext Create()
    {
        return SceneContext{
            CreateEngineContext()
        };
    }


};

class TestSystem : public ISystem
{
public:
inline static bool shutdownCalled = false;
bool initialized = false;
bool updated = false;
bool fixedUpdated = false;

TestSystem(entt::registry& reg) : ISystem(reg) {}

void Initialize(SceneContext&) override
{
    initialized = true;
}


void Update(SceneContext&, float) override
{
    updated = true;
}


void FixedUpdate(SceneContext&, float) override
{
    fixedUpdated = true;
}


void Shutdown(SceneContext&) override
{
    shutdownCalled = true;
}


};

class TestScene : public Scene
{
public:

bool created = false;
bool entered = false;
bool exited = false;
bool paused = false;
bool resumed = false;


protected:

void OnCreate() override
{
    created = true;
    AddSystem<TestSystem>(Registry());
}


void OnEnter() override
{
    entered = true;
}


void OnExit() override
{
    exited = true;
}


void OnPause() override
{
    paused = true;
}


void OnResume() override
{
    resumed = true;
}


};