#pragma once
#include "math_custom/Vector4.h"
#include "math_custom/Vector3.h"
#include "render/backend/Shader.h"
#include "resources/assets/Texture.h"
class Shader;

struct RenderMaterial
{
    Texture* albedo = nullptr;
    Texture* arm = nullptr;
    Texture* normal = nullptr;
    Texture* emissive = nullptr;

    Vector4 baseColorFactor;
    Vector3 emissiveFactor;

    float metallic;
    float roughness;
    float ao;

    void Bind(Shader* shader) const;

};
