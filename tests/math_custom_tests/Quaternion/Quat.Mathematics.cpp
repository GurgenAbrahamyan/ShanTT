#include <gtest/gtest.h>

#include "math_custom/Quat.h"

TEST(QuatTest, LengthSquared)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.lengthSquared(), 30.0f);
}

TEST(QuatTest, Length)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.length(), std::sqrt(30.0f));
}

TEST(QuatTest, Normalize)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    q.normalize();

    EXPECT_NEAR(q.length(), 1.0f, 1e-5f);
}

TEST(QuatTest, Normalized)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    Quat result = q.normalized();

    EXPECT_NEAR(result.length(), 1.0f, 1e-5f);

    // original should stay unchanged
    EXPECT_EQ(q, Quat(1.0f,2.0f,3.0f,4.0f));
}

TEST(QuatTest, DotProduct)
{
    Quat q1{1,2,3,4};
    Quat q2{5,6,7,8};

    EXPECT_FLOAT_EQ(q1.dot(q2), 70.0f);
}

TEST(QuatTest, Conjugate)
{
    Quat q{1.0f, -2.0f, 3.0f, 4.0f};

    EXPECT_EQ(
        q.conjugate(),
        Quat(-1.0f, 2.0f, -3.0f, 4.0f)
    );
}

TEST(QuatTest, Inverse)
{
    Quat q{1.0f, 2.0f, 3.0f, 4.0f};

    Quat inverse = q.inverse();

    EXPECT_TRUE(
        (q * inverse).nearEqual(Quat{})
    );
}

TEST(QuatTest, RotateVector)
{
    Quat q = Quat::fromAxisAngleDeg(
        Vector3{0,0,1},
        90.0f
    );

    Vector3 result = q.rotate(Vector3{1,0,0});

    EXPECT_TRUE(
        result.nearEqual(Vector3{0,1,0})
    );
}

TEST(QuatTest, InverseRotateVector)
{
    Quat q = Quat::fromAxisAngleDeg(
        Vector3{0,0,1},
        90.0f
    );

    Vector3 rotated = q.rotate(Vector3{1,0,0});

    Vector3 original = q.inverseRotate(rotated);

    EXPECT_TRUE(
        original.nearEqual(Vector3{1,0,0})
    );
}

TEST(QuatTest, FromAxisAngle)
{
    Quat q = Quat::fromAxisAngleDeg(
        Vector3{0,0,1},
        90.0f
    );

    Vector3 result = q.rotate(Vector3{1,0,0});

    EXPECT_TRUE(
        result.nearEqual(Vector3{0,1,0})
    );
}

TEST(QuatTest, EulerConversion) {



    Vector3 euler{30.0f, 45.0f, 60.0f};

    Quat q1 = Quat::fromEulerDeg(euler);

    Vector3 converted = q1.toEulerDeg();

    Quat q2 = Quat::fromEulerDeg(converted);

    EXPECT_TRUE(q1.sameRotation(q2));
}

