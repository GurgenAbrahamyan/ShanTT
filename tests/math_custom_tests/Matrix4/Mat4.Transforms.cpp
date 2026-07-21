#include <gtest/gtest.h>
#include "math_custom/Mat4.h"


TEST(Mat4Test, Translate)
{
    Vector3 translation{5.0f, -3.0f, 2.0f};

    Mat4 m = Mat4::translate(translation);

    Vector3 result = m.multiplyVec(Vector3{1.0f, 2.0f, 3.0f});

    EXPECT_TRUE(result.nearEqual(Vector3{6.0f, -1.0f, 5.0f}));
}


TEST(Mat4Test, Scale)
{
    Vector3 scale{2.0f, 3.0f, 4.0f};

    Mat4 m = Mat4::scale(scale);

    Vector3 result = m.multiplyVec(Vector3{1.0f, 2.0f, 3.0f});

    EXPECT_TRUE(result.nearEqual(Vector3{2.0f, 6.0f, 12.0f}));
}


TEST(Mat4Test, RotateX90)
{
    Mat4 m = Mat4::rotateX(90.0f);

    Vector3 result = m.multiplyVec(Vector3{0.0f, 1.0f, 0.0f});

    EXPECT_TRUE(result.nearEqual(Vector3{0.0f, 0.0f, 1.0f}));
}


TEST(Mat4Test, RotateY90)
{
    Mat4 m = Mat4::rotateY(90.0f);

    Vector3 result = m.multiplyVec(Vector3{0.0f, 0.0f, 1.0f});

    EXPECT_TRUE(result.nearEqual(Vector3{1.0f, 0.0f, 0.0f}));
}


TEST(Mat4Test, RotateZ90)
{
    Mat4 m = Mat4::rotateZ(90.0f);

    Vector3 result = m.multiplyVec(Vector3{1.0f, 0.0f, 0.0f});

    EXPECT_TRUE(result.nearEqual(Vector3{0.0f, 1.0f, 0.0f}));
}