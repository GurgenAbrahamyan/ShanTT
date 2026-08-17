#pragma once
#include "IExtractionSystem.h"
#include "resources/managers/TextureManager.h"

class GlobalTextureExtractor : public IExtractionSystem
{
public:
    explicit GlobalTextureExtractor(
        const TextureManager& textureManager,
        TextureID brdfLUT
    )
        : m_TextureManager(textureManager)
        , m_BrdfLUT(brdfLUT)
    {
    }

    void extract(
        entt::registry& registry,
        FrameRenderData& frameRenderData
    ) override;

private:
    const TextureManager& m_TextureManager;
    TextureID m_BrdfLUT;
};