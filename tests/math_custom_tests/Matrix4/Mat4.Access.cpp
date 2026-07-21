#include <gtest/gtest.h>

#include "math_custom/Mat4.h"

TEST(Mat4Test, IndexingOperator){

    float data[16]{1,  2,  3,  4,
         5,  6,  7,  8,
         9,  10, 11, 12,
         13, 14, 15, 16};
    Mat4 mat1{ data };

    for (size_t i{}; i < 16; ++i){
        EXPECT_FLOAT_EQ(mat1[i], data[i]);
    }

    EXPECT_DEATH(mat1[16], "");
}

TEST(Mat4Test, ConstIndexingOperator){

    float data[16]{1,  2,  3,  4,
         5,  6,  7,  8,
         9,  10, 11, 12,
         13, 14, 15, 16};
    const Mat4 mat1{ data };

    for (size_t i{}; i < 16; ++i){
        EXPECT_FLOAT_EQ(mat1[i], data[i]);
    }
    
    EXPECT_DEATH(mat1[16], "");
}