#include <gtest/gtest.h>
#include "../TestUtility.h"

#include "scene/SceneManager.h"

TEST(SceneManager, UpdatesCurrentScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& scene = manager.PushScene<TestScene>();


    manager.Update(0.016f);


    auto* system = scene.GetSystem<TestSystem>();


    ASSERT_NE(system, nullptr);

    EXPECT_TRUE(system->updated);
}

TEST(SceneManager, FixedUpdatesCurrentScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& scene = manager.PushScene<TestScene>();


    manager.FixedUpdate(0.016f);


    auto* system = scene.GetSystem<TestSystem>();


    ASSERT_NE(system, nullptr);

    EXPECT_TRUE(system->fixedUpdated);
}