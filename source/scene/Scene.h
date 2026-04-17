#pragma once
#include <vector>
#include "../../include/EnTT/entt.hpp"

class Texture;
class EventBus;
class ModelManager;
class TextureManager;
class MaterialManager;
class MeshManager;
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

    TextureManager* textureManager = nullptr;
    MaterialManager* materialManager = nullptr;
    MeshManager* meshManager = nullptr;
    ModelManager* modelManager = nullptr;
};