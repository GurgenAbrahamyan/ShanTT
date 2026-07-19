#include <gtest/gtest.h>

#include "math_custom/Vector2.h"


TEST(Vector2Test, DotProduct)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{4.0f, 5.0f};

    float result = v1.dot(v2);

    EXPECT_FLOAT_EQ(result, 14.0f);

    EXPECT_NEAR((Vector2(1, 0).dot(Vector2{0, 1})), 0.0f, 1e-5f);
}

TEST(Vector2Test, CrossProduct)
{
    Vector2 v1{1.0f, 2.0f};
    Vector2 v2{4.0f, 5.0f};

    float result = v1.cross(v2);

    EXPECT_FLOAT_EQ(result, -3.0f);

    EXPECT_GT((Vector2(1,0).cross(Vector2{0,1})), 0.0f);
    EXPECT_LT((Vector2(0,1).cross(Vector2{1,0})), 0.0f);
}



TEST(Vector2Test, VectorLength)
{
    Vector2 v{3.0f, 4.0f};

    float result = v.length();

    EXPECT_FLOAT_EQ(result, 5.0f);

    EXPECT_FLOAT_EQ((Vector2(0,0).length()), 0.0f);
}


TEST(Vector2Test, VectorLengthSquared)
{
    Vector2 v{3.0f, 4.0f};

    float result = v.lengthSquared();

    EXPECT_FLOAT_EQ(result, 25.0f);
}


TEST(Vector2Test, NormalizedVector)
{
    Vector2 v{3.0f, 0.0f};

    Vector2 result = v.normalized();

    EXPECT_TRUE(result.nearEqual(Vector2{1.0f, 0.0f}, 1e-5f));

    // normalized vector should have length 1
    EXPECT_NEAR(result.length(), 1.0f, 1e-5f);

    EXPECT_DEATH( (Vector2(0.0f, 0.0f).normalized()), "Cannot normalize near-zero vector");
}


TEST(Vector2Test, PerpendicularVector)
{
    Vector2 v{1.0f, 2.0f};

    Vector2 result = v.perpendicular(Direction::CW);

    EXPECT_TRUE(result.nearEqual(Vector2{2.0f,-1.0f}, 1e-5f));

    Vector2 result2 = v.perpendicular(Direction::CCW);

    EXPECT_TRUE(result2.nearEqual(Vector2(-2.0f, 1.0f), 1e-5f));

}

TEST(Vector2Test, PerpendicularIsOrthogonal)
{
    Vector2 v{3.0f, 7.0f};

    Vector2 perp = v.perpendicular(Direction::CCW);

    EXPECT_NEAR(v.dot(perp), 0.0f, 1e-5f);
}

TEST(Vector2Test, PerpendicularPreservesLength)
{
    Vector2 v{3.0f, 7.0f};

    Vector2 perp = v.perpendicular(Direction::CCW);

    EXPECT_NEAR(v.length(), perp.length(), 1e-5f);
}

TEST(Vector2Test, PerpendicularTwiceNegatesVector)
{
    Vector2 v{3.0f, 7.0f};

    Vector2 result = v.perpendicular(Direction::CCW)
                     .perpendicular(Direction::CCW);

    EXPECT_TRUE(result.nearEqual(Vector2(-3.0f,-7.0f), 1e-5f));
}