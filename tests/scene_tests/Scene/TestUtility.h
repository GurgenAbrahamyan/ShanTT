#pragma once

#include "scene/Scene.h"

struct TestContext
{
    EventBus events;
    entt::registry registry;

    SceneContext Create()
    {
        return SceneContext{
            events,
            registry
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
    bool exited = false;
    bool entered = false;
    

protected:
    void OnCreate() override
    {
        AddSystem<TestSystem>();
    }

    void OnEnter() override
    {
        entered = true;
    }

    void OnExit() override
    {
        exited = true;
    }
    
};