#pragma once

#include "scene/Scene.h"


struct TestContext
{
    EngineContext events;
    entt::registry registry;

    SceneContext Create()
    {
        return SceneContext{
            event(),
            &registry
        };
    }

    private:
        EngineContext event() { return 

            EngineContext{
                nullptr,
                0.0f,
                0.0f,
                events.events,
                events.renderer,
                events.physics,
                events.assets,
                events.input
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
    bool entered = false;
    bool exited = false;
    bool paused = false;
    bool resumed = false;


protected:

    void OnCreate() override
    {
        created = true;
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


    void OnPause() override
    {
        paused = true;
    }


    void OnResume() override
    {
        resumed = true;
    }
};