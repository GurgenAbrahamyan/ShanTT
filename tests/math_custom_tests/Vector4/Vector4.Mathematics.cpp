#include <gtest/gtest.h>

#include "math_custom/Vector4.h"


TEST(Vector4Test, DotProduct)
{
    Vector4 v1{1.0f, 2.0f, 3.0f, 4.0F};
    Vector4 v2{4.0f, 5.0f, 6.0f, 4.0f};

    float result = v1.dot(v2);

    EXPECT_FLOAT_EQ(result, 32.0f);
}


TEST(Vector4Test, VectorLength)
{
    Vector4 v{6.0f, 6.0f, 0.0f, 3.0f};

    float result = v.length();

    EXPECT_FLOAT_EQ(result, 9.0f);
}


TEST(Vector4Test, VectorLengthSquared)
{
    Vector4 v{3.0f, 4.0f, 0.0f, 10};

    float result = v.lengthSquared();

    EXPECT_FLOAT_EQ(result, 125.0f);
}


TEST(Vector4Test, NormalizedVector)
{
    Vector4 v{3.0f, 0.0f, 0.0f, 0.0f};

    Vector4 result = v.normalized();

    EXPECT_TRUE(result.nearEqual(Vector4{1.0f, 0.0f, 0.0f, 0.0}, 1e-5f));

    // normalized vector should have length 1
    EXPECT_NEAR(result.length(), 1.0f, 1e-5f);

    EXPECT_DEATH( (Vector4(0.0f, 0.0f, 0.0f, 0.0f).normalized()), "Cannot normalize near-zero vector");
}