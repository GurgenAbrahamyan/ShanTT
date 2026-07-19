#include <gtest/gtest.h>

#include "math_custom/Vector4.h"

TEST(Vector4Test, EqualityOperator)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 v2{1.0f, 2.0f, 3.0f, 4.0f};
    //Uses equality operator to compare v1 and v2
    EXPECT_EQ(v1, v2);
   
}

TEST(Vector4Test, InequalityOperator)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 v2{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 v3{4.0f, 5.0f, 6.0f, 7.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
}

TEST(Vector4Test, NearEqualityComparison)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0F};
    Vector4 v2{1.00001f, 2.00001f, 3.00001f, 4.00001f};

    EXPECT_TRUE(v1.nearEqual(v2, 0.0001f));
    Vector4 v3{1.001f, 2.001f, 3.001f, 4.001f};

    EXPECT_FALSE(v1.nearEqual(v3, 0.0001f));
}