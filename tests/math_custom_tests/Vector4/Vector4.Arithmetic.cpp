#include <gtest/gtest.h>

#include "math_custom/Vector4.h"

TEST(Vector4Test, AdditionOperators)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 v2{4.0f, 5.0f, 6.0f, 7.0f};

    Vector4 result{v1 + v2};

    EXPECT_EQ(result, (Vector4{5.0f, 7.0f, 9.0f, 11.0f}));

    v1 += v2;

    EXPECT_EQ(v1, (Vector4{5.0f, 7.0f, 9.0f, 11.0f}));
}

TEST(Vector4Test, AdditionOperatorsFloatingPoint)
{
    Vector4 v1{0.1f, 0.2f, 0.3f, 0.4};
    Vector4 v2{0.2f, 0.3f, 0.4f, 0.5f};

    Vector4 expected{0.3f, 0.5f, 0.7f, 0.9f};


    EXPECT_TRUE((v1 + v2).nearEqual(expected));

    v1 += v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector4Test, Subtraction)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 v2{4.0f, 5.0f, 6.0f, 7.0f};

    Vector4 result{v1 - v2};

    EXPECT_EQ(result, (Vector4{-3.0f, -3.0f, -3.0f, -3.0f}));

    v1 -= v2;

    EXPECT_EQ(v1, (Vector4{-3.0f, -3.0f, -3.0f, -3.0f}));
}

TEST(Vector4Test, SubtractionFloatingPoint)
{
    Vector4 v1{0.1f, 0.2f, 0.3f, 0.4f};
    Vector4 v2{0.2f, 0.3f, 0.4f, 0.5f};

    Vector4 expected{-0.1f, -0.1f, -0.1f, -0.1f};


    EXPECT_TRUE((v1 - v2).nearEqual(expected));

    v1 -= v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector4Test, Multiplication)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0F};
    float scalar = 2.0f;

    Vector4 result{v1 * scalar};

    EXPECT_EQ(result, (Vector4{2.0f, 4.0f, 6.0f, 8.0F}));

    result = scalar * v1;

    EXPECT_EQ(result, (Vector4{2.0f, 4.0f, 6.0f, 8.0f}));

    v1 *= scalar;

    EXPECT_EQ(v1, (Vector4{2.0f, 4.0f, 6.0f, 8.0f}));
}

TEST(Vector4Test, MultiplicationFloatingPoint)
{
    Vector4 v1{0.1f, 0.2f, 0.3f, 0.4f};
    float scalar = 0.3f;

    Vector4 expected{0.03f, 0.06f, 0.09f, 0.12f};

    Vector4 result{v1 * scalar};

    EXPECT_TRUE(result.nearEqual(expected));

    result = scalar * v1;

    EXPECT_TRUE(result.nearEqual(expected));

    v1 *= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}


TEST(Vector4Test, Division)
{
    Vector4 v1{2.0f, 4.0f, 6.0f, 8.0f};
    float scalar = 2.0f;

    Vector4 result{v1 / scalar};

    EXPECT_EQ(result, (Vector4{1.0f, 2.0f, 3.0f, 4.0f}));


    v1 /= scalar;

    EXPECT_EQ(v1, (Vector4{1.0f, 2.0f, 3.0f, 4.0f}));
}

TEST(Vector4Test, DivisionFloatingPoint)
{
    Vector4 v1{0.1f, 0.2f, 0.3f, 0.4f};
    float scalar = 0.2f;

    Vector4 expected{0.5f, 1.0f, 1.5f, 2.0f};

    Vector4 result{v1 / scalar};

    EXPECT_TRUE(result.nearEqual(expected));


    v1 /= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}

TEST(Vector4Test, DivisionByZero)
{
    Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_DEATH(v / 0.0f, "Division by zero or near-zero");

    EXPECT_DEATH(
        v /= 0.0f,
        "Division by zero or near-zero"
    );
}

TEST(Vector4Test, NegativeOperator)
{
    Vector4 v{1.0f, -2.0f, 3.0f, -4.0f};

    Vector4 result{-v};

    EXPECT_EQ(result, (Vector4{-1.0f, 2.0f, -3.0f, 4.0f}));
}