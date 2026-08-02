#include <gtest/gtest.h>

#include "TestUtility.h"


TEST(SceneLifecycle, EntersScene)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    EXPECT_EQ(
        scene.GetState(),
        Scene::SceneState::Exited
    );

    scene.Enter();

    EXPECT_EQ(
        scene.GetState(),
        Scene::SceneState::Active
    );

    EXPECT_TRUE(scene.entered);
}


TEST(SceneLifecycle, ExitsScene)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    scene.Enter();
    scene.Exit();

    EXPECT_TRUE(scene.exited);
}