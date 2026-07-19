#include <gtest/gtest.h>

#include "math_custom/Vector2.h"

TEST(Vector2Test, EqualityOperator)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{1.0f, 2.0f};
    Vector2 v3{4.0f, 5.0f};

    //Uses equality operator to compare v1 and v2
    EXPECT_EQ(v1, v2);
   
}

TEST(Vector2Test, InequalityOperator)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{1.0f, 2.0f};
    Vector2 v3{4.0f, 5.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
}

TEST(Vector2Test, NearEqualityComparison)
{
    Vector2 v1{1.0f, 2.0};
    Vector2 v2{1.00001f, 2.00001f};

    EXPECT_TRUE(v1.nearEqual(v2, 0.0001f));

    Vector2 v3{1.001f, 2.001f};

    EXPECT_FALSE(v1.nearEqual(v3, 0.0001f));
}