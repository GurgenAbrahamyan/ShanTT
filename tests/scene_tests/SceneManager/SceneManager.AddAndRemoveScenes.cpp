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