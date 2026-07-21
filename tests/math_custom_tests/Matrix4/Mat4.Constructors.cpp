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
    for(size_t row {}; row < 4; ++row)
        for(size_t col {}; col < 4; ++col)
            EXPECT_FLOAT_EQ(mat(row, col), expected[row * 4 + col]);
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

    for(size_t row {}; row < 4; ++row)
        for(size_t col {}; col < 4; ++col)
            EXPECT_FLOAT_EQ(mat(row, col), values[row * 4 + col]);
}

TEST(Mat4Test, 4Vec4Constructor)
{
    Mat4 mat{{1,  2,  3,  4}, 
             {5,  6,  7,  8}, 
             {9,  10, 11, 12},
             {13, 14, 15, 16}
            };

    constexpr float expected[16] =
    {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16
    };

    for(size_t row {}; row < 4; ++row)
        for(size_t col {}; col < 4; ++col)
            EXPECT_FLOAT_EQ(mat(row, col), expected[row * 4 + col]);
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
