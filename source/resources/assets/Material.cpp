
#include "Material.h"
#include "Texture.h"

#include "../../render/backend/Shader.h"


void Material::SetTexture(unsigned int slot, Texture* texture) {
    if (static_cast<size_t>(slot) >= textures.size()) {
        textures.resize(slot + 1, nullptr);
    }
    textures[slot] = texture;
}

Texture* Material::GetTexture(unsigned int slot) const {
    if (static_cast<size_t>(slot) >= 0 && slot < textures.size()) {
        return textures[slot];
    }
    return nullptr;
}

void Material::Bind(Shader* shader) const {
    if (!shader) return;
    static const char* slotNames[] = {
        "albedoMap",    // 0 BASE_COLOR
        "armMap",       // 1 ARM
        "normalMap",    // 2 NORMAL_MAP
        "emissiveMap",  // 3 EMISSIVE
        "heightMap",    // 4 HEIGHT
    };
    for (int i = 0; i < (int)textures.size(); i++) {
        if (!textures[i]) continue;
        textures[i]->Bind(i);
        shader->setInt(slotNames[i], i);
    }

    shader->setFloat("metallicFactor", metallic);
    shader->setFloat("roughnessFactor", roughness);
    shader->setFloat("aoFactor", ao);
    shader->setFloat("heightScale", heightScale);
    shader->setVec4("baseColorFactor", baseColorFactor);
    shader->setVec3("emissiveFactor", emissiveFactor);
}
