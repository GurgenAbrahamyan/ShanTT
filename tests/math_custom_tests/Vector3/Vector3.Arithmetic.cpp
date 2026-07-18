#include <gtest/gtest.h>

#include "math_custom/Vector3.h"

TEST(Vector3Test, AdditionOperators)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{4.0f, 5.0f, 6.0f};

    Vector3 result{v1 + v2};

    EXPECT_EQ(result, (Vector3{5.0f, 7.0f, 9.0f}));

    v1 += v2;

    EXPECT_EQ(v1, (Vector3{5.0f, 7.0f, 9.0f}));
}

TEST(Vector3Test, AdditionOperatorsFloatingPoint)
{
    Vector3 v1{0.1f, 0.2f, 0.3f};
    Vector3 v2{0.2f, 0.3f, 0.4f};

    Vector3 expected{0.3f, 0.5f, 0.7f};


    EXPECT_TRUE((v1 + v2).nearEqual(expected));

    v1 += v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector3Test, Subtraction)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{4.0f, 5.0f, 6.0f};

    Vector3 result{v1 - v2};

    EXPECT_EQ(result, (Vector3{-3.0f, -3.0f, -3.0f}));

    v1 -= v2;

    EXPECT_EQ(v1, (Vector3{-3.0f, -3.0f, -3.0f}));
}

TEST(Vector3Test, SubtractionFloatingPoint)
{
     Vector3 v1{0.1f, 0.2f, 0.3f};
    Vector3 v2{0.2f, 0.3f, 0.4f};

    Vector3 expected{-0.1f, -0.1f, -0.1f};


    EXPECT_TRUE((v1 - v2).nearEqual(expected));

    v1 -= v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector3Test, Multiplication)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    float scalar = 2.0f;

    Vector3 result{v1 * scalar};

    EXPECT_EQ(result, (Vector3{2.0f, 4.0f, 6.0f}));

    result = scalar * v1;

    EXPECT_EQ(result, (Vector3{2.0f, 4.0f, 6.0f}));

    v1 *= scalar;

    EXPECT_EQ(v1, (Vector3{2.0f, 4.0f, 6.0f}));
}

TEST(Vector3Test, MultiplicationFloatingPoint)
{
    Vector3 v1{0.1f, 0.2f, 0.3f};
    float scalar = 0.3f;

    Vector3 expected{0.03f, 0.06f, 0.09f};

    Vector3 result{v1 * scalar};

    EXPECT_TRUE(result.nearEqual(expected));

    result = scalar * v1;

    EXPECT_TRUE(result.nearEqual(expected));

    v1 *= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}


TEST(Vector3Test, Division)
{
    Vector3 v1{2.0f, 4.0f, 6.0f};
    float scalar = 2.0f;

    Vector3 result{v1 / scalar};

    EXPECT_EQ(result, (Vector3{1.0f, 2.0f, 3.0f}));


    v1 /= scalar;

    EXPECT_EQ(v1, (Vector3{1.0f, 2.0f, 3.0f}));
}

TEST(Vector3Test, DivisionFloatingPoint)
{
    Vector3 v1{0.1f, 0.2f, 0.3f};
    float scalar = 0.2f;

    Vector3 expected{0.5f, 1.0f, 1.5f};

    Vector3 result{v1 / scalar};

    EXPECT_TRUE(result.nearEqual(expected));


    v1 /= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}