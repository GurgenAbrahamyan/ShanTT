#pragma once
#include <entt.hpp>

#include "resources/managers/ModelManager.h"
#include "resources/managers/MeshManager.h"
#include "resources/managers/MaterialManager.h"
#include "resources/managers/TextureManager.h"

#include "scene/IExtractionSystem.h"
#include "render/data/FrameRenderData.h"

class SceneExtractor : public IExtractionSystem
{
public:
    SceneExtractor(
        const ModelManager& modelManager,
        const MeshManager& meshManager,
        const MaterialManager& materialManager,
        const TextureManager& textureManager
    )
        : m_modelManager(modelManager)
        , m_meshManager(meshManager)
        , m_materialManager(materialManager)
        , m_textureManager(textureManager)
    {}

    void extract(
        entt::registry& registry,
        FrameRenderData& out
    ) override;

private:
    [[maybe_unused]] const ModelManager& m_modelManager;
    const MeshManager& m_meshManager;
    const MaterialManager& m_materialManager;
    const TextureManager& m_textureManager;
    static Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);
};