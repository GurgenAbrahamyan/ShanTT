#include <gtest/gtest.h>
#include "../TestUtility.h"



TEST(SceneSystemManagement, UpdatesSystems)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    scene.Enter();

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

    scene.Enter();

    scene.FixedUpdate(0.016f);


    auto* system = scene.GetSystem<TestSystem>();

    EXPECT_TRUE(system->fixedUpdated);
}
