#include "EnvironmentExtractor.h"
#include "EnvironmentRenderData.h"
#include "ecs/components/graphics/CubeMapComponent.h"
void EnvironmentExtractor::extract(
    entt::registry& registry,
    FrameRenderData& frameRenderData)
{
    auto view = registry.view<CubeMapComponent>();

    if (view.empty())
        return;

    const auto entity = view.front();
    const auto& cubeMap = view.get<CubeMapComponent>(entity);

    const EnvironmentMap& environment = cubeMap.environment;

    EnvironmentRenderData& data =
        frameRenderData.Emplace<EnvironmentRenderData>();

    data.env = 
        m_TextureManager.getCubeMap( environment.env);
        
    data.irr =
        m_TextureManager.getCubeMap(environment.irradiance);

    data.prefilter =
        m_TextureManager.getCubeMap(environment.prefiltered);

    data.intensity =
        cubeMap.intensity;

    data.dirLightInfluence =
        cubeMap.dirLightInfluence;
}