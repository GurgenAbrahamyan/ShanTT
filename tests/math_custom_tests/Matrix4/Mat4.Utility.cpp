#include <gtest/gtest.h>

#include "math_custom/Mat4.h"

TEST(Mat4Test, MatTranspose)
{
    Mat4 m1{{
        4.0f,  5.0f,  6.0f,  7.0f,
        8.0f,  9.0f,  10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f,
        16.0f, 17.0f, 18.0f, 19.0f
    }};

    Mat4 expected{{
        4.0f,  8.0f,  12.0f,  16.0f,
        5.0f,  9.0f,  13.0f, 17.0f,
        6.0f, 10.0f, 14.0f, 18.0f,
        7.0f, 11.0f, 15.0f, 19.0f
    }};
    EXPECT_EQ(m1.transpose(), expected);
}

TEST(Mat4Test, MatDeterminant)
{
    Mat4 m1{{
        4.0f,  5.0f,  6.0f,  7.0f,
        8.0f,  9.0f,  10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f,
        16.0f, 17.0f, 18.0f, 19.0f
    }};

    float expected { 0.0f };
    EXPECT_EQ(m1.determinant(), expected);
}

TEST(Mat4Test, MatInverse)
{
    Mat4 m1{{
        1.0f, 0.0f, 2.0f, 3.0f,
        0.0f, 4.0f, 1.0f, 0.0f,
        2.0f, 1.0f, 5.0f, 2.0f,
        3.0f, 0.0f, 2.0f, 6.0f
    }};

    EXPECT_TRUE((m1.inverse() * m1).nearEqual(Mat4{}));

    Mat4 m2{{
        4.0f,  5.0f,  6.0f,  7.0f,
        8.0f,  9.0f,  10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f,
        16.0f, 17.0f, 18.0f, 19.0f
    }};

    EXPECT_DEATH((m2.inverse()), "");
}

TEST(Mat4Test, FromQuatIdentity)
{
    Quat q{0.0f, 0.0f, 0.0f, 1.0f};

    Mat4 expected{{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    EXPECT_TRUE(Mat4::fromQuat(q).nearEqual(expected));
}


TEST(Mat4Test, FromQuatRotationZ90)
{
    float halfAngle = Mat4::radians(90.0f) / 2.0f;

    Quat q{
        0.0f,
        0.0f,
        sin(halfAngle),
        cos(halfAngle)
    };

    Mat4 expected{{
         0.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  0.0f, 0.0f, 0.0f,
         0.0f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.0f, 0.0f, 1.0f
    }};

    EXPECT_TRUE(Mat4::fromQuat(q).nearEqual(expected));
}
