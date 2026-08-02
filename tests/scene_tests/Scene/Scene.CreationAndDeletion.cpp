#include <gtest/gtest.h>

#include "TestUtility.h"


TEST(SceneSystemManagement, AddsAndInitializesSystem)
{
    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    auto* system = scene.GetSystem<TestSystem>();

    ASSERT_NE(system, nullptr);
    EXPECT_TRUE(system->initialized);
}

TEST(SceneSystemManagement, ShutsDownSystems)
{
    TestSystem::shutdownCalled = false;

    TestContext test;
    auto ctx = test.Create();

    TestScene scene;

    scene.Initialize(ctx);

    scene.Shutdown();

    EXPECT_TRUE(TestSystem::shutdownCalled);
}