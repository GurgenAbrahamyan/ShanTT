
#pragma once
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector2.h"
struct Vertex {
    Vector3 position;
    Vector3 color;
	Vector2 uv;
    Vector3 normal;
    Vector3 tangent = { 1,0,0 };
    float tangentW = 1.0f;
};
