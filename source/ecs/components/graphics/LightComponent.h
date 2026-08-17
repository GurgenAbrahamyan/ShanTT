#pragma once
#include "../../../math_custom/Vector3.h"

enum class LightType {
    Point = 0,
    Directional = 1,
    Spot = 2
};

struct LightComponent {
    
    LightType type = LightType::Point;
    Vector3 color = Vector3(1, 1, 1);
    float intensity = 1.0f;
    float innerConeAngle = 0.9f;
    float outerConeAngle = 0.95f;

    bool castsShadow = false;
    float shadowNearPlane = -1.0f;
    float shadowFarPlane  = -1.0f;
    float shadowOrthoSize = -1.0f; // directional only
};