#pragma once
#include <entt/entt.hpp>

#include "resources/managers/AssetManager.h"

#include "scene/IExtractionSystem.h"
#include "render/data/FrameRenderData.h"

namespace JPH { class BodyInterface; }

class SkeletalPoseResolveSystem : public IExtractionSystem
{
public:
    SkeletalPoseResolveSystem(
        AssetManager& assetManager,
        JPH::BodyInterface& bodyInterface)
        : assets(assetManager)
        , m_bodyInterface(bodyInterface)
    {}

    void extract(
        entt::registry& registry,
        FrameRenderData& out
    ) override;

private:
    AssetManager& assets;
    JPH::BodyInterface& m_bodyInterface;
};