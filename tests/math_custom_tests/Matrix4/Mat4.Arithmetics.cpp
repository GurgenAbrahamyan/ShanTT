#include <gtest/gtest.h>

#include "math_custom/Mat4.h"


TEST(Mat4Test, AdditionOperators)
{
    Mat4 m1{{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    }};

    Mat4 m2{{
        4.0f, 5.0f, 6.0f, 7.0f,
        8.0f, 9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f,
        16.0f, 17.0f, 18.0f, 19.0f
    }};

    Mat4 expected{{
        5.0f, 7.0f, 9.0f, 11.0f,
        13.0f, 15.0f, 17.0f, 19.0f,
        21.0f, 23.0f, 25.0f, 27.0f,
        29.0f, 31.0f, 33.0f, 35.0f
    }};

    EXPECT_EQ(m1 + m2, expected);

    m1 += m2;

    EXPECT_EQ(m1, expected);
}


TEST(Mat4Test, AdditionOperatorsFloatingPoint)
{
    Mat4 m1{{
        0.1f, 0.2f, 0.3f, 0.4f,
        0.5f, 0.6f, 0.7f, 0.8f,
        0.9f, 1.0f, 1.1f, 1.2f,
        1.3f, 1.4f, 1.5f, 1.6f
    }};

    Mat4 m2{{
        0.2f, 0.3f, 0.4f, 0.5f,
        0.6f, 0.7f, 0.8f, 0.9f,
        1.0f, 1.1f, 1.2f, 1.3f,
        1.4f, 1.5f, 1.6f, 1.7f
    }};

    Mat4 expected{{
        0.3f, 0.5f, 0.7f, 0.9f,
        1.1f, 1.3f, 1.5f, 1.7f,
        1.9f, 2.1f, 2.3f, 2.5f,
        2.7f, 2.9f, 3.1f, 3.3f
    }};

    EXPECT_TRUE((m1 + m2).nearEqual(expected));

    m1 += m2;

    EXPECT_TRUE(m1.nearEqual(expected));
}


TEST(Mat4Test, SubtractionOperators)
{
    Mat4 m1{{
        4.0f, 5.0f, 6.0f, 7.0f,
        8.0f, 9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f,
        16.0f, 17.0f, 18.0f, 19.0f
    }};

    Mat4 m2{{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    }};

    Mat4 expected{{
        3.0f, 3.0f, 3.0f, 3.0f,
        3.0f, 3.0f, 3.0f, 3.0f,
        3.0f, 3.0f, 3.0f, 3.0f,
        3.0f, 3.0f, 3.0f, 3.0f
    }};

    EXPECT_EQ(m1 - m2, expected);

    m1 -= m2;

    EXPECT_EQ(m1, expected);
}

TEST(Mat4Test, MatrixMultiplicationIdentity)
{
    Mat4 m{
        {
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f
        }
    };

    Mat4 identity{};

    EXPECT_TRUE((m * identity).nearEqual(m));
    EXPECT_TRUE((identity * m).nearEqual(m));
}

TEST(Mat4Test, MatrixMultiplication)
{
    Mat4 a{
        {
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f
        }
    };

    Mat4 b{
        {
            16.0f, 15.0f, 14.0f, 13.0f,
            12.0f, 11.0f, 10.0f, 9.0f,
            8.0f, 7.0f, 6.0f, 5.0f,
            4.0f, 3.0f, 2.0f, 1.0f
        }
    };

    Mat4 expected{
        {
            80.0f, 70.0f, 60.0f, 50.0f,
            240.0f, 214.0f, 188.0f, 162.0f,
            400.0f, 358.0f, 316.0f, 274.0f,
            560.0f, 502.0f, 444.0f, 386.0f
        }
    };

    EXPECT_TRUE((a * b).nearEqual(expected));

    a *= b;

    EXPECT_TRUE(a.nearEqual(expected));
}

TEST(Mat4Test, Multiplication)
{
    Mat4 m{{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    }};

    float scalar = 2.0f;

    Mat4 expected{{
        2.0f, 4.0f, 6.0f, 8.0f,
        10.0f, 12.0f, 14.0f, 16.0f,
        18.0f, 20.0f, 22.0f, 24.0f,
        26.0f, 28.0f, 30.0f, 32.0f
    }};

    EXPECT_EQ(m * scalar, expected);

    EXPECT_EQ(scalar * m, expected);

    m *= scalar;

    EXPECT_EQ(m, expected);
}


TEST(Mat4Test, Division)
{
    Mat4 m{{
        2.0f, 4.0f, 6.0f, 8.0f,
        10.0f, 12.0f, 14.0f, 16.0f,
        18.0f, 20.0f, 22.0f, 24.0f,
        26.0f, 28.0f, 30.0f, 32.0f
    }};

    float scalar = 2.0f;

    Mat4 expected{{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    }};

    EXPECT_EQ(m / scalar, expected);

    m /= scalar;

    EXPECT_EQ(m, expected);
}


TEST(Mat4Test, DivisionByZero)
{
    Mat4 m{};

    EXPECT_DEATH(m / 0.0f, "Division by zero or near-zero");

    EXPECT_DEATH(
        m /= 0.0f,
        "Division by zero or near-zero"
    );
}


TEST(Mat4Test, NegativeOperator)
{
    Mat4 m{{
        1.0f, -2.0f, 3.0f, -4.0f,
        5.0f, -6.0f, 7.0f, -8.0f,
        9.0f, -10.0f, 11.0f, -12.0f,
        13.0f, -14.0f, 15.0f, -16.0f
    }};

    Mat4 expected{{
        -1.0f, 2.0f, -3.0f, 4.0f,
        -5.0f, 6.0f, -7.0f, 8.0f,
        -9.0f, 10.0f, -11.0f, 12.0f,
        -13.0f, 14.0f, -15.0f, 16.0f
    }};

    EXPECT_EQ(-m, expected);
}