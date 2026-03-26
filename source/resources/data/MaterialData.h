#pragma once
#include <string>
#include <vector>
#include "MaterialTextureInfo.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector4.h"
struct MaterialData {
    std::string name;
    std::vector<MaterialTextureInfo> textureInfo;

	float heightScale = 1.0f; 
    float metallic = 1.0f;
    float roughness = 1.0f;
    float ao = 1.0f;

   
    Vector4 baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    Vector3 emissiveFactor = { 1.0f, 1.0f, 1.0f };
};
