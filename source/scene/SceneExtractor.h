#pragma once
#include <entt.hpp>

#include "resources/managers/ModelManager.h"
#include "resources/managers/MeshManager.h"
#include "resources/managers/MaterialManager.h"
#include "resources/managers/TextureManager.h"
#include "resources/managers/SkeletonManager.h"

#include "scene/IExtractionSystem.h"
#include "render/data/FrameRenderData.h"

class SceneExtractor : public IExtractionSystem
{
public:
    SceneExtractor(
        const ModelManager& modelManager,
        const MeshManager& meshManager,
        const MaterialManager& materialManager,
        const TextureManager& textureManager,
        const SkeletonManager& skeletonManager
    )
        : m_modelManager(modelManager)
        , m_meshManager(meshManager)
        , m_materialManager(materialManager)
        , m_textureManager(textureManager)
        , m_skeletonManager(skeletonManager)
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
    const SkeletonManager& m_skeletonManager;

    static Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);

    entt::entity findSkeletonOwner(
    entt::entity entity,
    entt::registry& registry);
    

    Mat4 boneGlobalMatrix(
    const Skeleton& skeleton,
    uint32_t boneId);
};