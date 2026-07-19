#include <gtest/gtest.h>
#include "math_custom/Vector2.h"
#include "math_custom/Vector3.h"
#include "math_custom/Vector4.h"

#include <utility>

TEST(Vector4Test, EmptyConstructor)
{
    Vector4 v{};
    EXPECT_EQ(v.x, 0.0f);
    EXPECT_EQ(v.y, 0.0f);
    EXPECT_EQ(v.z, 0.0f);
    EXPECT_EQ(v.w, 0.0f);
}

TEST(Vector4Test, Vector2Constructor)
{
    Vector4 v { Vector2{1.0f, 2.0f}, 3.0f, 4.0f};
    EXPECT_EQ(v.x, 1.0f);
    EXPECT_EQ(v.y, 2.0f);
    EXPECT_EQ(v.z, 3.0f);
    EXPECT_EQ(v.w, 4.0f);
}

TEST(Vector4Test, Vector3Constructor)
{
    Vector4 v {Vector3{1.0f, 2.0f, 3.0f}, 4.0f};
    EXPECT_EQ(v.x, 1.0f);
    EXPECT_EQ(v.y, 2.0f);
    EXPECT_EQ(v.z, 3.0f);
    EXPECT_EQ(v.w, 4.0f);
}

TEST(Vector4Test, CopyConstructor)
{
    Vector4 v {1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 copy{v};
    
    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
    EXPECT_EQ(copy.z, 3.0f);
    EXPECT_EQ(copy.w, 4.0f);
}

TEST(Vector4Test, CopyAssignment)
{
    Vector4 v {1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 copy{};
    copy = v;

    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
    EXPECT_EQ(copy.z, 3.0f);
    EXPECT_EQ(copy.w, 4.0f);
}

TEST(Vector4Test, MoveConstructor)
{
    Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 move{std::move(v)};
    
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
    EXPECT_EQ(move.z, 3.0f);
    EXPECT_EQ(move.w, 4.0f);
}
TEST(Vector4Test, MoveAssignment)
{
    Vector4 v(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 move{};
    move = std::move(v);
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
    EXPECT_EQ(move.z, 3.0f);
    EXPECT_EQ(move.w, 4.0f);
}






