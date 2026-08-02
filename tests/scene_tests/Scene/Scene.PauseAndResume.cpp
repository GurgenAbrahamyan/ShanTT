#include <gtest/gtest.h>
#include "TestUtility.h"


TEST(SceneLifecycle, PausesScene)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);
    scene.Enter();

    scene.Pause();

    EXPECT_EQ(
        scene.GetState(),
        Scene::SceneState::Paused
    );

    EXPECT_TRUE(scene.paused);
}


TEST(SceneLifecycle, ResumesScene)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);
    scene.Enter();

    scene.Pause();
    scene.Resume();

    EXPECT_EQ(
        scene.GetState(),
        Scene::SceneState::Active
    );

    EXPECT_TRUE(scene.resumed);
}


TEST(SceneLifecycle, PausedSceneDoesNotUpdateSystems)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);
    scene.Enter();

    auto* system = scene.GetSystem<TestSystem>();

    ASSERT_NE(system, nullptr);

    scene.Pause();

    scene.Update(0.016f);

    EXPECT_FALSE(system->updated);
}


TEST(SceneLifecycle, ResumedSceneUpdatesSystems)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);
    scene.Enter();

    auto* system = scene.GetSystem<TestSystem>();

    ASSERT_NE(system, nullptr);

    scene.Pause();
    scene.Resume();

    scene.Update(0.016f);

    EXPECT_TRUE(system->updated);
}