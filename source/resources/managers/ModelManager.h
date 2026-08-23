#pragma once

#include <string>
#include <unordered_map>

#include "ResourcePool.h"
#include "ModelHandleTypes.h"
#include "../../resources/data/ModelAssetDef.h"

class MeshManager;
class MaterialManager;
class TextureManager;
class SkeletonManager;

class ModelManager {
public:
    ModelManager(
        MeshManager* meshManager,
        MaterialManager* materialManager,
        TextureManager* textureManager,
        SkeletonManager* skeletonManager
    );

    ModelAssetID loadModel(const std::string& name, const std::string& path);

    const ModelAssetDef* getModel(ModelAssetID id) const;
    ModelAssetID getModelID(const std::string& name) const;

    bool isLoaded(const std::string& name) const;

private:
    MeshManager*     meshManager;
    MaterialManager* materialManager;
    TextureManager*  textureManager;
    SkeletonManager* skeletonManager;

    ResourcePool<ModelAssetDef, ModelAssetTag> pool;
    std::unordered_map<std::string, ModelAssetID> lookup; // keyed by name
};