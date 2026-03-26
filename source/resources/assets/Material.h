#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector4.h"
class Texture;
class Shader;
class Material {
public:
    Material() = default;

    void SetTexture(int slot, Texture* texture);
    Texture* GetTexture(int slot) const;

    float metallic = 1.0f;
    float roughness = 1.0f;
    float ao = 1.0f;
    float heightScale = 0.005f;

    Vector4 baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    Vector3 emissiveFactor = { 1.0f, 1.0f, 1.0f };

    void Bind(Shader* shader) const;
    int  getID() const { return ID; }
    void setID(int id) { ID = id; }
    void setHeightScale(float scale) { heightScale = scale; }
private:
    std::vector<Texture*> textures;
    int ID = -1;
};