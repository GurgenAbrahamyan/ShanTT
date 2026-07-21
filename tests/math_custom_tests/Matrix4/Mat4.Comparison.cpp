#include <gtest/gtest.h>

#include "math_custom/Mat4.h"

TEST(Mat4Test, EqualOperator){

    Mat4 mat1{};
    Mat4 mat2{};

    EXPECT_TRUE(mat1 == mat2);

    mat2(1, 2) = 7.0f;

    EXPECT_FALSE ( mat1 == mat2);
}

TEST(Mat4Test, InEqualOperator){

    Mat4 mat1{};
    Mat4 mat2{};

    EXPECT_FALSE(mat1 != mat2);

    mat2(3, 2) = 7.0f;
    
    EXPECT_TRUE ( mat1 != mat2);
}

TEST(Mat4Test, NearEqual){

    Mat4 mat1{
        {1,  2,  3,  4,
         5,  6,  7,  8,
         9,  10, 11, 12,
         13, 14, 15, 16}
    };
    Mat4 mat2{
        {1.0000001f,  2.0000001f,  3.0000001f,  4.0000001f,
         5.0000001f,  6.0000001f,  7.0000001f,  8.0000001f,
         9.0000001f,  10.0000001f, 11.0000001f, 12.0000001f,
         13.0000001f, 14.0000001f, 15.0000001f, 16.0000001f}
    };
    
    
    EXPECT_TRUE ( mat1.nearEqual(mat2, 1e-5f));

    mat2(0, 0) = 1.0001f;

    EXPECT_FALSE ( mat1.nearEqual(mat2, 1e-5f));

}
