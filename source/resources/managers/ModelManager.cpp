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
entt::entity ModelManager::loadModel(
    const std::string& name,
    const std::string& path,
    entt::registry& registry,
    const ModelLoadConfig& config)
{
    
    ModelComponent component;
    if (auto it = loadedModels.find(name); it != loadedModels.end()) {
        std::cout << "Model already loaded: " << name << "\n";
        return it->second;
    }
    
    std::cout << "Loading model: " << name << " from " << path << "\n";

    ModelLoader loader(path);
    const ModelData& data = loader.getModelData();

    // ?? Create single root entity ??????????????????????????????
    auto root = registry.create();
    registry.emplace<TagComponent>(root, name);
    auto& modelComp = registry.emplace<ModelComponent>(root);

    // ?? Load materials ?????????????????????????????????????????
    std::vector<Material*> materialRefs;
    materialRefs.reserve(data.materials.size());

    for (const auto& matData : data.materials) {
        InitMaterial initEvent(const_cast<MaterialData*>(&matData));
        bus->publish<InitMaterial>(initEvent);

        Material* mat = initEvent.result;
        if (mat) {
            materialRefs.push_back(mat);
          
        }
        else {
            materialRefs.push_back(nullptr);
        }
    }

    // ?? Load meshes and attach to ModelComponent ??????????????
    for (size_t i = 0; i < data.submeshes.size(); i++) {
        const auto& submeshData = data.submeshes[i];

        // Mesh
        MeshData meshData = data.meshes[submeshData.meshIndex];
        meshData.name = name + "::" + submeshData.name;

        InitMesh meshEvent(&meshData);
        bus->publish<InitMesh>(meshEvent);

        RenderMesh* renderMesh = meshEvent.result;
        if (!renderMesh) continue;

        // Material
        Material* material = nullptr;
        if (submeshData.materialIndex != UINT32_MAX &&
            submeshData.materialIndex < materialRefs.size())
            material = materialRefs[submeshData.materialIndex];

        // Add to model component
        MeshEntry entry;
        entry.mesh = renderMesh;
        entry.material = material;
        entry.localTransform = submeshData.worldTransform; // store precomputed transform
        modelComp.meshes.push_back(entry);

     
    }

    loadedModels[name] = root;
    std::cout << "Model loaded: " << name << "\n";
    return root;
}

void ModelManager::destroyModel(entt::entity root, entt::registry& registry) {
    if (!registry.valid(root)) return;

    
    auto& modelComp = registry.get<ModelComponent>(root);
  //  for (int id : modelComp.meshIDs)     meshManager->unload(id);
   // for (int id : modelComp.materialIDs) materialManager->unload(id);
   // for (int id : modelComp.textureIDs)  textureManager->unload(id);

   
    registry.view<ParentComponent>().each([&](auto entity, auto& parent) {
        if (parent.parent == root)
            registry.destroy(entity);
        });

    for (auto it = loadedModels.begin(); it != loadedModels.end();) {
        if (it->second == root)
            it = loadedModels.erase(it);
        else
            ++it;
    }

    
    registry.destroy(root);
}

entt::entity ModelManager::getModel(const std::string& name) const {
    auto it = loadedModels.find(name);
    return it != loadedModels.end() ? it->second : entt::null;
}

bool ModelManager::isLoaded(const std::string& name) const {
    return loadedModels.find(name) != loadedModels.end();
}