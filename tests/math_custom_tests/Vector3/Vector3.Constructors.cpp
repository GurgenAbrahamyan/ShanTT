#include <gtest/gtest.h>
#include "math_custom/Vector3.h"

#include <utility>

TEST(Vector3Test, EmptyConstructor)
{
    Vector3 v{};
    EXPECT_EQ(v.x, 0.0f);
    EXPECT_EQ(v.y, 0.0f);
    EXPECT_EQ(v.z, 0.0f);
}

TEST(Vector3Test, FloatConstructor)
{
    Vector3 v {1.0f};
    EXPECT_EQ(v.x, 1.0f);
    EXPECT_EQ(v.y, 1.0f);
    EXPECT_EQ(v.z, 1.0f);
}

TEST(Vector3Test, FullConstructor)
{
    Vector3 v {1.0f, 2.0f, 3.0f};
    EXPECT_EQ(v.x, 1.0f);
    EXPECT_EQ(v.y, 2.0f);
    EXPECT_EQ(v.z, 3.0f);
}

TEST(Vector3Test, CopyConstructor)
{
    Vector3 v {1.0f, 2.0f, 3.0f};
    Vector3 copy{v};
    
    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
    EXPECT_EQ(copy.z, 3.0f);
}
TEST(Vector3Test, CopyAssignment)
{
    Vector3 v {1.0f, 2.0f, 3.0f};
    Vector3 copy{};
    copy = v;

    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
    EXPECT_EQ(copy.z, 3.0f);
}
TEST(Vector3Test, MoveConstructor)
{
    Vector3 v{1.0f, 2.0f, 3.0f};
    Vector3 move{std::move(v)};
    
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
    EXPECT_EQ(move.z, 3.0f);
}
TEST(Vector3Test, MoveAssignment)
{
    Vector3 v(1.0f, 2.0f, 3.0f);
    Vector3 move{};
    move = std::move(v);
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
    EXPECT_EQ(move.z, 3.0f);
}






