#include <gtest/gtest.h>
#include "../TestUtility.h"

#include "scene/SceneManager.h"

TEST(SceneManager, PushesScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& scene = manager.PushScene<TestScene>();


    EXPECT_TRUE(scene.created);
    EXPECT_TRUE(scene.entered);

    EXPECT_EQ(manager.Current(), &scene);
}

TEST(SceneManager, PushPausesPreviousScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& first = manager.PushScene<TestScene>();

    auto& second = manager.PushScene<TestScene>();


    EXPECT_TRUE(first.paused);

    EXPECT_TRUE(second.entered);


    EXPECT_EQ(manager.Current(), &second);
}

TEST(SceneManager, PopsScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& scene = manager.PushScene<TestScene>();


    manager.PopScene();


    EXPECT_TRUE(scene.exited);

    EXPECT_TRUE(manager.Empty());

    EXPECT_EQ(manager.Current(), nullptr);
}

TEST(SceneManager, PopResumesPreviousScene)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& gameplay = manager.PushScene<TestScene>();

    auto& menu = manager.PushScene<TestScene>();


    manager.PopScene();


    EXPECT_TRUE(menu.exited);

    EXPECT_TRUE(gameplay.resumed);


    EXPECT_EQ(manager.Current(), &gameplay);
}

TEST(SceneManager, HandlesMultipleScenes)
{
    TestContext test;
    auto ctx = test.Create();

    SceneManager manager(ctx);


    auto& a = manager.PushScene<TestScene>();
    auto& b = manager.PushScene<TestScene>();
    auto& c = manager.PushScene<TestScene>();


    manager.PopScene();
    EXPECT_EQ(manager.Current(), &b);


    manager.PopScene();
    EXPECT_EQ(manager.Current(), &a);


    manager.PopScene();
    EXPECT_TRUE(manager.Empty());
}
