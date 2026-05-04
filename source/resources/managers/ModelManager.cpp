#include "ModelManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "../../resources/loaders/ModelLoader.h"
#include "../../resources/assets/RenderMesh.h"
#include "../../resources/assets/Material.h"
#include "../../core/EventBus.h"
#include "../../core/Event.h"
#include "../data/MeshData.h"
#include "../data/MaterialData.h"

// Components
#include "../../ecs/components/core/TagComponent.h"
#include "../../ecs/components/core/WorldMatrixComponent.h"
#include "../../ecs/components/core/ParentComponent.h"
#include "../../ecs/components/graphics/MeshComponent.h"
#include "../../ecs/components/graphics/MaterialComponent.h"
#include "../../ecs/components/graphics/ModelComponent.h"

#include "../../resources/data/ModelLoadConfig.h"

#include <iostream>

ModelManager::ModelManager(
    EventBus* bus,
    MeshManager* meshManager,
    MaterialManager* materialManager,
    TextureManager* textureManager)
    : bus(bus)
    , meshManager(meshManager)
    , materialManager(materialManager)
    , textureManager(textureManager)
{
}
bool ModelManager::loadModel(const std::string& name, const std::string& path)
{
    if (loadedModels.contains(name)) {
        std::cout << "Model already loaded: " << name << "\n";
        return false;
    }

    ModelLoader loader(path);
    if(loader.isValid()) {

    } else {
        
        return false;
	}
    const ModelData& data = loader.getModelData();
    
    ModelAsset asset;
    asset.name = name;
    asset.path = path;

    std::vector<Material*> materialRefs;
    materialRefs.reserve(data.materials.size());

    for (const auto& matData : data.materials) {
        InitMaterial ev(const_cast<MaterialData*>(&matData));
        bus->publish(ev);
        materialRefs.push_back(ev.result);
    }

    for (const auto& sub : data.submeshes)
    {
        MeshData meshData = data.meshes[sub.meshIndex];
        meshData.name = name + "::" + sub.name;

        InitMesh meshEv(&meshData);
        bus->publish(meshEv);

        if (!meshEv.result) continue;

        MeshEntry entry;
        entry.mesh = meshEv.result;
        entry.material =
            (sub.materialIndex < materialRefs.size())
            ? materialRefs[sub.materialIndex]
            : nullptr;

        entry.localTransform = sub.worldTransform;

        asset.meshes.push_back(entry);
    }

    loadedModels[name] = std::make_unique<ModelAsset>(std::move(asset));

    std::cout << "Loaded model asset: " << name << "\n";
    return true;
}

void ModelManager::instantiateModel(
    const std::string& name,
    entt::registry& registry,
    entt::entity entity)
{
    auto it = loadedModels.find(name);
    if (it == loadedModels.end()) return;

    const ModelAsset& asset = *it->second;

    auto& model = registry.emplace_or_replace<ModelComponent>(entity);
   
    model.asset = &asset;
}

bool ModelManager::isLoaded(const std::string& name) const {
    return loadedModels.find(name) != loadedModels.end();
}

const std::unordered_map<std::string, std::unique_ptr<ModelAsset>>&
ModelManager::getLoadedModels() const {
    return loadedModels;
}