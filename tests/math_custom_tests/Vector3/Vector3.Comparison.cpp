#include <gtest/gtest.h>

#include "math_custom/Vector3.h"

TEST(Vector3Test, EqualityOperator)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{1.0f, 2.0f, 3.0f};
    Vector3 v3{4.0f, 5.0f, 6.0f};

    //Uses equality operator to compare v1 and v2
    EXPECT_EQ(v1, v2);
   
}

TEST(Vector3Test, InequalityOperator)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{1.0f, 2.0f, 3.0f};
    Vector3 v3{4.0f, 5.0f, 6.0f};

    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
}

TEST(Vector3Test, NearEqualityComparison)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{1.00001f, 2.00001f, 3.00001f};

    EXPECT_TRUE(v1.nearEqual(v2, 0.0001f));

    Vector3 v3{1.001f, 2.001f, 3.001f};

    EXPECT_FALSE(v1.nearEqual(v3, 0.0001f));
}