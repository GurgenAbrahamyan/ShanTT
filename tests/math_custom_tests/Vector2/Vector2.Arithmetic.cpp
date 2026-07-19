#include <gtest/gtest.h>

#include "math_custom/Vector2.h"

TEST(Vector2Test, AdditionOperators)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{4.0f, 5.0f};

    Vector2 result{v1 + v2};

    EXPECT_EQ(result, (Vector2{5.0f, 7.0f}));

    v1 += v2;

    EXPECT_EQ(v1, (Vector2{5.0f, 7.0f}));
}

TEST(Vector2Test, AdditionOperatorsFloatingPoint)
{
    Vector2 v1{0.1f, 0.2f};
    Vector2 v2{0.2f, 0.3f};

    Vector2 expected{0.3f, 0.5f};


    EXPECT_TRUE((v1 + v2).nearEqual(expected));

    v1 += v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector2Test, Subtraction)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{4.0f, 5.0f};

    Vector2 result{v1 - v2};

    EXPECT_EQ(result, (Vector2{-3.0f, -3.0f}));

    v1 -= v2;

    EXPECT_EQ(v1, (Vector2{-3.0f, -3.0f}));
}

TEST(Vector2Test, SubtractionFloatingPoint)
{
    Vector2 v1{0.1f, 0.2f};
    Vector2 v2{0.2f, 0.3f};

    Vector2 expected{-0.1f, -0.1f};


    EXPECT_TRUE((v1 - v2).nearEqual(expected));

    v1 -= v2;
    EXPECT_TRUE((v1).nearEqual(expected));
}

TEST(Vector2Test, Multiplication)
{
    Vector2 v1{1.0f, 2.0f};
    float scalar = 2.0f;

    Vector2 result{v1 * scalar};

    EXPECT_EQ(result, (Vector2{2.0f, 4.0f}));

    result = scalar * v1;

    EXPECT_EQ(result, (Vector2{2.0f, 4.0f}));

    v1 *= scalar;

    EXPECT_EQ(v1, (Vector2{2.0f, 4.0f}));
}

TEST(Vector2Test, MultiplicationFloatingPoint)
{
    Vector2 v1{0.1f, 0.2f};
    float scalar = 0.3f;

    Vector2 expected{0.03f, 0.06f};

    Vector2 result{v1 * scalar};

    EXPECT_TRUE(result.nearEqual(expected));

    result = scalar * v1;

    EXPECT_TRUE(result.nearEqual(expected));

    v1 *= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}


TEST(Vector2Test, Division)
{
    Vector2 v1{2.0f, 4.0f};
    float scalar = 2.0f;

    Vector2 result{v1 / scalar};

    EXPECT_EQ(result, (Vector2{1.0f, 2.0f}));


    v1 /= scalar;

    EXPECT_EQ(v1, (Vector2{1.0f, 2.0f}));
}

TEST(Vector2Test, DivisionFloatingPoint)
{
    Vector2 v1{0.1f, 0.2f};
    float scalar = 0.2f;

    Vector2 expected{0.5f, 1.0f};

    Vector2 result{v1 / scalar};

    EXPECT_TRUE(result.nearEqual(expected));


    v1 /= scalar;

    EXPECT_TRUE(v1.nearEqual(expected));
}

TEST(Vector2Test, DivisionByZero)
{
    Vector2 v{1.0f, 2.0f};

    EXPECT_DEATH(v / 0.0f, "Division by zero or near-zero");

    EXPECT_DEATH(
        v /= 0.0f,
        "Division by zero or near-zero"
    );
}

TEST(Vector2Test, NegativeOperator)
{
    Vector2 v{1.0f, -2.0f};

    Vector2 result{-v};

    EXPECT_EQ(result, (Vector2{-1.0f, 2.0f}));
}