#pragma once
#include "../../math_custom/Vector2.h"
#include "../../math_custom/Mat4.h"
struct ShadowData {
    Vector2 uvMin;
    Vector2 uvMax;
    Mat4 lightMatrix;
};