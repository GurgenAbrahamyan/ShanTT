#include "Material.h"
#include "Texture.h"
#include "resources/managers/TextureManager.h"
#include "../../render/backend/Shader.h"

void Material::Bind(Shader* shader, TextureManager& tm) const {
    if (!shader) return;

    static const char* slotNames[static_cast<size_t>(MaterialSlot::Count)] = {
        "albedoMap",   // Albedo
        "armMap",      // ARM
        "normalMap",   // Normal
        "emissiveMap", // Emissive
        "heightMap",   // Height
    };

    for (size_t i = 0; i < textures.size(); ++i) {
        MaterialSlot slot = static_cast<MaterialSlot>(i);
        Texture* tex = tm.getTexture(textures[i]);

        if (!tex) {
            TextureID fallback = (slot == MaterialSlot::Normal)
                ? tm.getDefaultNormal()
                : tm.getDefaultWhite();
            tex = tm.getTexture(fallback);
        }

        if (!tex) continue; 

        tex->Bind(static_cast<int>(i));
        shader->setInt(slotNames[i], static_cast<int>(i));
    }

    shader->setFloat("metallicFactor", metallic);
    shader->setFloat("roughnessFactor", roughness);
    shader->setFloat("aoFactor", ao);
    shader->setFloat("heightScale", heightScale);
    shader->setVec4("baseColorFactor", baseColorFactor);
    shader->setVec3("emissiveFactor", emissiveFactor);
}