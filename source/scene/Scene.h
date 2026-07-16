#pragma once
#include "EnTT/entt.hpp"
#include <memory>
#include "../resources/managers/TextureManager.h"
#include "../resources/managers/MaterialManager.h"
#include "../resources/managers/ModelManager.h"
#include "../resources/managers/MeshManager.h"

class Texture;
class EventBus;
class CubeMap;

class Scene {
public:
    Scene(EventBus* bus);
    ~Scene();

    void initObjects();

    entt::registry& getRegistry() { return registry; }
    CubeMap* getSkybox() const { return skybox; }
    Texture* getBRDF();
    ModelManager* getModelManager() const;
private:
    entt::registry   registry;
    CubeMap* skybox = nullptr;

    std::unique_ptr<TextureManager> textureManager = nullptr;
    std::unique_ptr<MaterialManager> materialManager = nullptr;
    std::unique_ptr<MeshManager> meshManager = nullptr;
    std::unique_ptr<ModelManager> modelManager = nullptr;
};
