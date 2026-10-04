#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

#include "resources/managers/ModelHandleTypes.h"
#include "resources/managers/SkeletonManager.h"

class ModelManager;

struct SpawnedModel
{

    entt::entity root = entt::null;

  
    std::unordered_map<std::string, entt::entity> partsByName;

    std::vector<entt::entity> sockets;
};

SpawnedModel spawnModel(
    const std::string& name,
    ModelAssetID assetId,
    ModelManager& models,
    SkeletonManager& skeletonManager,
    entt::registry& registry);