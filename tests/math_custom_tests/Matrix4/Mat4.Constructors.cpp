#include <gtest/gtest.h>

#include "math_custom/Mat4.h"

TEST(Mat4Test, DefaultConstructor)
{
    Mat4 mat{};
    constexpr float expected[16] =
    {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(mat[i], expected[i]);
    }
}

TEST(Mat4Test, FullConstructor)
{
    constexpr float values[16] =
    {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16
    };

    Mat4 mat(values);

    for (int i = 0; i < 16; ++i)
    {
        EXPECT_FLOAT_EQ(mat[i], values[i]);
    }
}

TEST(Mat4Test, CopyConstructor)
{
    Mat4 original({
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    });

    Mat4 copy(original);

    EXPECT_EQ(copy, original);
}

TEST(Mat4Test, CopyAssignment)
{
    Mat4 original({
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    });

    Mat4 copy{};

    copy = original;

    EXPECT_EQ(copy, original);
}
