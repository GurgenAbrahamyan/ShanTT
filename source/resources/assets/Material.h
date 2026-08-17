#pragma once
#include <array>

#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector4.h"
#include "resources/managers/TextureHandleTypes.h"

#include "resources/data/MaterialSlot.h"

class Texture;
class Shader;
class TextureManager;

class Material {
public:
    Material() = default;

    void SetTexture(MaterialSlot slot, TextureID id) { textures[static_cast<size_t>(slot)] = id; }
    TextureID GetTexture(MaterialSlot slot) const { return textures[static_cast<size_t>(slot)]; }

    float metallic = 1.0f;
    float roughness = 1.0f;
    float ao = 1.0f;
    float heightScale = 0.005f;

    Vector4 baseColorFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector3 emissiveFactor = { 1.0f, 1.0f, 1.0f };

    void Bind(Shader* shader, TextureManager& tm) const;

private:
    std::array<TextureID, static_cast<size_t>(MaterialSlot::Count)> textures{};
};