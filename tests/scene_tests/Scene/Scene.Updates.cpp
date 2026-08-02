#include <gtest/gtest.h>
#include "scene/Scene.h"
#include "core/ecs_systems/ISystem.h"


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

    bool initialized = false;
    bool updated = false;
    bool shutdown = false;
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
        shutdown = true;
    }
};


class TestScene : public Scene
{
protected:

    void OnCreate() override
    {
        AddSystem<TestSystem>();
    }
};




TEST(SceneSystemManagement, UpdatesSystems)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    scene.Update(0.016f);


    auto* system = scene.GetSystem<TestSystem>();

    ASSERT_NE(system, nullptr);

    EXPECT_TRUE(system->updated);
}

TEST(SceneSystemManagement, FixedUpdatesSystems)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    scene.FixedUpdate(0.016f);


    auto* system = scene.GetSystem<TestSystem>();

    EXPECT_TRUE(system->fixedUpdated);
}
