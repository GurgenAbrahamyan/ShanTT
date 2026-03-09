#pragma once
#include "Material.h"
#include "Texture.h"
#include <iostream>
#include "../../render/backend/Shader.h"


void Material::SetTexture(int slot, Texture* texture) {
    if (slot >= textures.size()) {
        textures.resize(slot + 1, nullptr);
    }
    textures[slot] = texture;
}

Texture* Material::GetTexture(int slot) const {
    if (slot >= 0 && slot < textures.size()) {
        return textures[slot];
    }
    return nullptr;
}

void Material::Bind(Shader* shader) const {
    if (!shader) return;

    shader->Activate(); 
    for (int i = 0; i < textures.size(); i++) {
        if (!textures[i]) continue;

       
        textures[i]->Bind(i);
     
        std::string uniformName = "tex" + std::to_string(i);

       
        GLint loc = glGetUniformLocation(shader->ID, uniformName.c_str());
        if (loc >= 0)
            glUniform1i(loc, i);
    }

  
 /*   GLint metallicLoc = glGetUniformLocation(shader->ID, "metallic");
    if (metallicLoc >= 0) glUniform1f(metallicLoc, metallic);

    GLint roughnessLoc = glGetUniformLocation(shader->ID, "roughness");
    if (roughnessLoc >= 0) glUniform1f(roughnessLoc, roughness);

    GLint aoLoc = glGetUniformLocation(shader->ID, "ao");
    if (aoLoc >= 0) glUniform1f(aoLoc, ao);*/
}
