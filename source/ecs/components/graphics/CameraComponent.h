#pragma once
#include "../../../math_custom/Vector3.h"
#include "../../../math_custom/Mat4.h"

struct CameraComponent {

    float fov = 120.0f;
    float aspectRatio = 1920.0f / 1200.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float pitch = 0.0f;
    float yaw = 0.0f;

    Vector3 front = Vector3(0, 0, -1); 
    Vector3 up = Vector3(0, 1, 0);
    Vector3 right = Vector3(1, 0, 0);


    // Depth of field / post-processing
    bool applyBlur = false;
    float focusDistance = 2.650f;
    float aperture = 0.670;
    float focalLength = 0.094f;
    float blurScale = 106.0f;

    // Matrices ready for OpenGL
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
};