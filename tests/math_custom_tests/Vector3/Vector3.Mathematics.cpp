#include <gtest/gtest.h>

#include "math_custom/Vector3.h"


TEST(Vector3Test, DotProduct)
{
    Vector3 v1{1.0f, 2.0f, 3.0f};
    Vector3 v2{4.0f, 5.0f, 6.0f};

    float result = v1.dot(v2);

    EXPECT_FLOAT_EQ(result, 32.0f);
}


TEST(Vector3Test, CrossProduct)
{
    Vector3 v1{1.0f, 0.0f, 0.0f};
    Vector3 v2{0.0f, 1.0f, 0.0f};

    Vector3 result = v1.cross(v2);

    EXPECT_EQ(result, (Vector3{0.0f, 0.0f, 1.0f}));
}


TEST(Vector3Test, VectorLength)
{
    Vector3 v{3.0f, 4.0f, 0.0f};

    float result = v.length();

    EXPECT_FLOAT_EQ(result, 5.0f);
}


TEST(Vector3Test, VectorLengthSquared)
{
    Vector3 v{3.0f, 4.0f, 0.0f};

    float result = v.lengthSquared();

    EXPECT_FLOAT_EQ(result, 25.0f);
}


TEST(Vector3Test, NormalizedVector)
{
    Vector3 v{3.0f, 0.0f, 0.0f};

    Vector3 result = v.normalized();

    EXPECT_TRUE(result.nearEqual(Vector3{1.0f, 0.0f, 0.0f}, 1e-5f));

    // normalized vector should have length 1
    EXPECT_NEAR(result.length(), 1.0f, 1e-5f);
}


TEST(Vector3Test, OrthogonalVector)
{
    Vector3 v{1.0f, 2.0f, 3.0f};

    Vector3 result = v.orthogonal();

    // A perpendicular vector has a dot product of zero
    EXPECT_NEAR(v.dot(result), 0.0f, 1e-5f);
}