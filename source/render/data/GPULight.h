#pragma once
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Mat4.h"
struct GPULight {
    int type;
    float intensity;
    float pad0, pad1;
    Vector3 color;   float pad2;
    Vector3 position; float pad3;
    Vector3 direction; float pad4;
    float innerCone, outerCone;
    int shadowIndex = -1; // index into ShadowAtlas::tiles, -1 = no shadow
    int pad5;

};