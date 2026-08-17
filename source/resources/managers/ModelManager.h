#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <entt/entt.hpp>

#include "../../resources/data/ModelAsset.h"

class MeshManager;
class MaterialManager;
class TextureManager;

class ModelManager {
public:
    ModelManager(
        MeshManager* meshManager,
        MaterialManager* materialManager,
        TextureManager* textureManager
    );

    bool loadModel(
        const std::string& name,
        const std::string& path
    );

    void instantiateModel(
        const std::string& name,
        entt::registry& registry,
        entt::entity entity
    );

    const std::unordered_map<
        std::string,
        std::unique_ptr<ModelAsset>
    >& getLoadedModels() const;

    bool isLoaded(const std::string& name) const;

private:
    MeshManager* meshManager;
    MaterialManager* materialManager;
    TextureManager* textureManager;

    std::unordered_map<
        std::string,
        std::unique_ptr<ModelAsset>
    > loadedModels;
};