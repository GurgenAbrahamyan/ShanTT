#include "GlobalTexturesExtractor.h"

#include "GlobalTexturesRenderData.h"

void GlobalTextureExtractor::extract(
    entt::registry&,
    FrameRenderData& frameRenderData)
{
    GlobalTextureRenderData& data =
        frameRenderData.Emplace<GlobalTextureRenderData>();

    data.brdfLUT =
        m_TextureManager.getTexture(m_BrdfLUT);
}