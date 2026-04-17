#pragma once
#include <unordered_map>
#include <string>
#include "../../include/EnTT/entt.hpp"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"
#include "../../resources/data/ModelLoadConfig.h" 
#include "../../resources/data/ModelAsset.h"

class EventBus;
class MeshManager;
class MaterialManager;
class TextureManager;
class ModelComponent;



class ModelManager {
public:
    ModelManager(
        EventBus* bus,
        MeshManager* meshManager,
        MaterialManager* materialManager,
        TextureManager* textureManager
    );

   
    bool loadModel(const std::string& name, const std::string& path);

    void instantiateModel(
        const std::string& name,
        entt::registry& registry,
        entt::entity entity
    );

    const std::unordered_map<std::string, ModelAsset>& getLoadedModels() const;

    bool isLoaded(const std::string& name) const;

    





private:
    EventBus* bus;
    MeshManager* meshManager;
    MaterialManager* materialManager;
    TextureManager* textureManager;

    std::unordered_map<std::string, ModelAsset> loadedModels;
};