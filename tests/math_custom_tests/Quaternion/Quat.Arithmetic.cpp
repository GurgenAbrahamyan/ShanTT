#include <gtest/gtest.h>

#include "math_custom/Quat.h"

TEST(QuatTest, Addition)
{
    Quat q1{1.0f, 2.0f, 3.0f, 1.0f};
    Quat q2{1.0f, 2.0f, 3.0f, 1.0f};

    EXPECT_EQ(q1 + q2, Quat(2.0f, 4.0f, 6.0f, 2.0f));

    q1 += q2;

    EXPECT_EQ(q1, Quat(2.0f, 4.0f, 6.0f, 2.0f));
}

TEST(QuatTest, FloatingPointAddition)
{
    Quat q1{1.234f, 2.345f, 3.456f, 1.234f};
    Quat q2{1.001f, 2.002f, 3.003f, 1.001f};

    EXPECT_TRUE( (q1 + q2).nearEqual(Quat(2.235f, 4.347f, 6.459f, 2.235f), 1e-5f));

    q1 += q2;

    EXPECT_TRUE( q1.nearEqual(Quat(2.235f, 4.347f, 6.459f, 2.235f), 1e-5f));

}

TEST(QuatTest, Subtraction)
{
    Quat q1{1.0f, 2.0f, 3.0f, 1.0f};
    Quat q2{1.0f, 3.0f, 7.0f, 2.0f};

    EXPECT_EQ(q1 - q2, Quat(0.0f, -1.0f, -4.0f, -1.0f));

    q1 -= q2;

    EXPECT_EQ(q1, Quat(0.0f, -1.0f, -4.0f, -1.0f));

}

TEST(QuatTest, FloatingPointSubtraction)
{
    Quat q1{1.234f, 2.345f, 3.456f, 1.234f};
    Quat q2{1.001f, 2.002f, 3.003f, 1.001f};

    EXPECT_TRUE( (q1 - q2).nearEqual(Quat(0.233f, 0.343f, 0.453f, 0.233f), 1e-5f));

    q1 -= q2;

    EXPECT_TRUE( q1.nearEqual(Quat(0.233f, 0.343f, 0.453f, 0.233f), 1e-5f));

}


TEST(QuatTest, QuaternionMultiplication)
{
    Quat q{0.2f, 0.3f, 0.4f, 0.5f};
    Quat identity{};

    Quat result1 = q * identity;
    Quat result2 = identity * q;

    EXPECT_TRUE(result1.nearEqual(q));
    EXPECT_TRUE(result2.nearEqual(q));
}

TEST(QuatTest, HamiltonMultiplicationIsNotCommutative)
{
    Quat q1{0.1f, 0.2f, 0.3f, 0.4f};
    Quat q2{0.5f, 0.6f, 0.7f, 0.8f};

    Quat result1 = q1 * q2;
    Quat result2 = q2 * q1;

    EXPECT_FALSE(result1.nearEqual(result2));
}

TEST(QuatTest, ScalarMultiplication)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_EQ(q * 2.0f, Quat(2.0f, 4.0f, 6.0f, 8.0f));

    q *= 2.0f;

    EXPECT_EQ(q, Quat(2.0f, 4.0f, 6.0f, 8.0f));

    q = 2.0f * q;

    EXPECT_EQ(q, Quat(4.0f, 8.0f, 12.0f, 16.0f));

}


TEST(QuatTest, FloatingPointScalarMultiplication)
{
    Quat q{1.234f, 2.345f, 3.456f, 4.567f};

    EXPECT_TRUE(
        (q * 1.5f).nearEqual(
            Quat(1.851f, 3.5175f, 5.184f, 6.8505f)
        )
    );

    q *= 1.5f;

    EXPECT_TRUE(
        q.nearEqual(
            Quat(1.851f, 3.5175f, 5.184f, 6.8505f)
        )
    );
}


TEST(QuatTest, ScalarDivision)
{
    Quat q{2.0f, 4.0f, 6.0f, 8.0f};

    EXPECT_EQ(q / 2.0f, Quat(1.0f, 2.0f, 3.0f, 4.0f));

    q /= 2.0f;

    EXPECT_EQ(q, Quat(1.0f, 2.0f, 3.0f, 4.0f));
}


TEST(QuatTest, FloatingPointScalarDivision)
{
    Quat q{1.234f, 2.345f, 3.456f, 4.567f};

    EXPECT_TRUE(
        (q / 2.0f).nearEqual(
            Quat(0.617f, 1.1725f, 1.728f, 2.2835f)
        )
    );

    q /= 2.0f;

    EXPECT_TRUE(
        q.nearEqual(
            Quat(0.617f, 1.1725f, 1.728f, 2.2835f)
        )
    );
}


TEST(QuatTest, DivisionByNearZero)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_DEATH(q / 1e-10f, "Division by zero or near-zero");

    EXPECT_DEATH(q /= 1e-10f, "Division by zero or near-zero");
}


TEST(QuatTest, NegativeOperator)
{
    Quat q{1.0f, -2.0f, 3.0f, -4.0f};

    EXPECT_EQ(-q, Quat(-1.0f, 2.0f, -3.0f, 4.0f));
}