#include <gtest/gtest.h>
#include "math_custom/Vector2.h"

#include <utility>

TEST(Vector2Test, EmptyConstructor)
{
    Vector2 v{};
    EXPECT_EQ(v.x, 0.0f);
    EXPECT_EQ(v.y, 0.0f);
}

TEST(Vector2Test, FullConstructor)
{
    Vector2 v {1.0f, 2.0f};
    EXPECT_EQ(v.x, 1.0f);
    EXPECT_EQ(v.y, 2.0f);
  
}

TEST(Vector2Test, CopyConstructor)
{
    Vector2 v {1.0f, 2.0f};
    Vector2 copy{v};
    
    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
}
TEST(Vector2Test, CopyAssignment)
{
    Vector2 v {1.0f, 2.0f};
    Vector2 copy{};
    copy = v;

    EXPECT_EQ(copy.x, 1.0f);
    EXPECT_EQ(copy.y, 2.0f);
}
TEST(Vector2Test, MoveConstructor)
{
    Vector2 v{1.0f, 2.0f};
    Vector2 move{std::move(v)};
    
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
   
}
TEST(Vector2Test, MoveAssignment)
{
    Vector2 v(1.0f, 2.0f);
    Vector2 move{};
    move = std::move(v);
    EXPECT_EQ(move.x, 1.0f);
    EXPECT_EQ(move.y, 2.0f);
}






