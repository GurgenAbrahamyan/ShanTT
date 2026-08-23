#pragma once
#include <string>
#include <unordered_map>
#include <entt/entt.hpp>
#include "resources/managers/ModelHandleTypes.h"

class ModelManager;

struct SpawnedModel {
    entt::entity root = entt::null;
    std::unordered_map<std::string, entt::entity> partsByName;
};

SpawnedModel spawnModel(const std::string& name, ModelAssetID assetId, ModelManager& models, entt::registry& registry);