#include <gtest/gtest.h>

#include "math_custom/Quat.h"

TEST(QuatTest, EqualityOperator)
{
    Quat q1{0.0f, 1.0f, 2.0f, 1.0f};
    Quat q2{0.0f, 1.0f, 2.0f, 1.0f};

    EXPECT_TRUE(q1 == q2);
    EXPECT_FALSE(q1 == Quat{});
}

TEST(QuatTest, InEqualityOperator)
{
    Quat q1{0.0f, 1.0f, 2.0f, 1.0f};
    Quat q2{0.0f, 1.0f, 2.0f, 1.0f};

    EXPECT_FALSE(q1 != q2);
    EXPECT_TRUE (q1 != Quat{});

  
}

TEST(QuatTest, NearEqual)
{
    Quat q1{0.0f, 1.0f, 2.0f, 1.0f};
    Quat q2{0.000001f, 1.000001f, 2.000001f, 1.000001f};
    Quat q3{0.001f, 1.001f, 2.001f, 1.001f};
    EXPECT_TRUE(q1.nearEqual(q2, 1e-5f));
    EXPECT_FALSE(q1.nearEqual(q3, 1e-5f));

  
}