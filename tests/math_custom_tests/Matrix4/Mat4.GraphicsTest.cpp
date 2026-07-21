
#include <gtest/gtest.h>
#include "math_custom/Mat4.h"


TEST(Mat4GraphicsTest, PerspectiveCenterProjection)
{
    Mat4 projection = Mat4::perspective(
        90.0f,
        1.0f,
        1.0f,
        10.0f
    );

    Vector4 point{
        0.0f,
        0.0f,
        -1.0f,
        1.0f
    };

    Vector4 result = projection.multiplyVec(point);

    float ndcZ = result.z / result.w;

    EXPECT_NEAR(ndcZ, -1.0f, 1e-5f);
}


TEST(Mat4GraphicsTest, PerspectiveFarPlane)
{
    Mat4 projection = Mat4::perspective(
        90.0f,
        1.0f,
        1.0f,
        10.0f
    );

    Vector4 point{
        0.0f,
        0.0f,
        -10.0f,
        1.0f
    };

    Vector4 result = projection.multiplyVec(point);

    float ndcZ = result.z / result.w;

    EXPECT_NEAR(ndcZ, 1.0f, 1e-5f);
}


TEST(Mat4GraphicsTest, OrthographicCenter)
{
    Mat4 projection = Mat4::ortho(
        -10.0f,
        10.0f,
        -10.0f,
        10.0f,
        1.0f,
        100.0f
    );

    Vector4 point{
        0.0f,
        0.0f,
        -50.5f,
        1.0f
    };

    Vector4 result = projection.multiplyVec(point);

    EXPECT_NEAR(result.x, 0.0f, 1e-5f);
    EXPECT_NEAR(result.y, 0.0f, 1e-5f);
    EXPECT_NEAR(result.z, 0.0f, 1e-5f);
}


TEST(Mat4GraphicsTest, OrthographicBounds)
{
    Mat4 projection = Mat4::ortho(
        -1.0f,
        1.0f,
        -1.0f,
        1.0f,
        -1.0f,
        1.0f
    );

    Vector4 point{
        1.0f,
        1.0f,
        -1.0f,
        1.0f
    };

    Vector4 result = projection.multiplyVec(point);

    EXPECT_NEAR(result.x, 1.0f, 1e-5f);
    EXPECT_NEAR(result.y, 1.0f, 1e-5f);
}


TEST(Mat4GraphicsTest, LookAtCameraTranslation)
{
    Vector3 eye{
        0.0f,
        0.0f,
        5.0f
    };

    Vector3 center{
        0.0f,
        0.0f,
        0.0f
    };

    Vector3 up{
        0.0f,
        1.0f,
        0.0f
    };


    Mat4 view = Mat4::lookAt(
        eye,
        center,
        up
    );


    Vector3 result = view.multiplyVec(
        Vector3{0.0f, 0.0f, 0.0f}
    );


    EXPECT_TRUE(
        result.nearEqual(
            Vector3{0.0f, 0.0f, -5.0f}
        )
    );
}


TEST(Mat4GraphicsTest, LookAtForwardDirection)
{
    Vector3 eye{
        0.0f,
        0.0f,
        0.0f
    };

    Vector3 center{
        0.0f,
        0.0f,
        -1.0f
    };

    Vector3 up{
        0.0f,
        1.0f,
        0.0f
    };


    Mat4 view = Mat4::lookAt(
        eye,
        center,
        up
    );


    Vector3 result = view.multiplyVec(
        Vector3{0.0f, 0.0f, -1.0f}
    );


    EXPECT_TRUE(
        result.nearEqual(
            Vector3{0.0f, 0.0f, -1.0f}
        )
    );
}