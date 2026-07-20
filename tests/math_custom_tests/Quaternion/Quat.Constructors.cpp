#include <gtest/gtest.h>
#include "math_custom/Quat.h"

TEST(QuatTest, EmptyConstructor) {

    Quat q{};
    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);
  
}

TEST(Quat, ParameterizedConstructor)
{
    Quat q(1.5f, -2.0f, 3.25f, -4.75f);

    EXPECT_FLOAT_EQ(q.x, 1.5f);
    EXPECT_FLOAT_EQ(q.y, -2.0f);
    EXPECT_FLOAT_EQ(q.z, 3.25f);
    EXPECT_FLOAT_EQ(q.w, -4.75f);
}
