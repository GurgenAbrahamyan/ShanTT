#include "RenderMaterialData.h"
#include "MaterialSlot.h"
void RenderMaterial::Bind(Shader* shader) const
{
    if (!shader)
        return;

    static const char* slotNames[
        static_cast<size_t>(MaterialSlot::Count)
    ] = {
        "albedoMap",
        "armMap",
        "normalMap",
        "emissiveMap",
        "heightMap"
    };

    Texture* textures[
        static_cast<size_t>(MaterialSlot::Count)
    ] = {
        albedo,
        arm,
        normal,
        emissive
    };

    for (size_t i = 0; i < static_cast<size_t>(MaterialSlot::Count); ++i)
    {
        Texture* texture = textures[i];

        if (!texture)
            continue;

        texture->Bind(static_cast<int>(i));
        shader->setInt(
            slotNames[i],
            static_cast<int>(i)
        );
    }

    shader->setFloat(
        "metallicFactor",
        metallic
    );

    shader->setFloat(
        "roughnessFactor",
        roughness
    );

    shader->setFloat(
        "aoFactor",
        ao
    );

    /*shader->setFloat(
        "heightScale",
        heightScale
    );*/

    shader->setVec4(
        "baseColorFactor",
        baseColorFactor
    );

    shader->setVec3(
        "emissiveFactor",
        emissiveFactor
    );
}