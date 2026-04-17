#pragma once
#include "../../../math_custom/Vector3.h"
#include "../../../math_custom/Quat.h";

struct TransformComponent {
    Vector3 position;
    Quat rotation;
    Vector3 scale = Vector3(1,1,1);
};