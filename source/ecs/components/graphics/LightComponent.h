// LightComponent.h
#pragma once
#include "../../../math_custom/Vector3.h"
#include "../../../math_custom/Mat4.h"
enum class LightType {
    Point = 0,
    Directional = 1,
    Spot = 2
};

struct LightComponent {
    LightType type = LightType::Point;
    Vector3 color = Vector3(1, 1, 1);
    Vector3 direction = Vector3(0, 0, -1);
    float intensity = 1.0f;
    float innerConeAngle = 0.9f;
    float outerConeAngle = 0.95f;

	Mat4 lightProjection = Mat4();
    bool castsShadow = false; 
};

