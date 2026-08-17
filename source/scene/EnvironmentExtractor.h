#pragma once

#include "IExtractionSystem.h"
#include "resources/managers/TextureManager.h"

class EnvironmentExtractor : public IExtractionSystem
{
public:
    explicit EnvironmentExtractor(const TextureManager& textureManager)
        : m_TextureManager(textureManager)
    {
    }

    void extract(
        entt::registry& registry,
        FrameRenderData& frameRenderData) override;

private:
    const TextureManager& m_TextureManager;
};