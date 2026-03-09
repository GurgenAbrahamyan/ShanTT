#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "../assets/Material.h"
class EventBus;
class TextureManager;
struct MaterialData;
// Define standard texture slot indices
enum class TextureSlot {
    BASE_COLOR = 0,
    NORMAL_MAP = 1,
    METALLIC_ROUGHNESS = 2,
    OCCLUSION = 3,
    EMISSIVE = 4,
    MAX_SLOTS = 5
};

class MaterialManager {
public:
    MaterialManager(TextureManager* texMgr, EventBus* bus);

    int addMaterial(const MaterialData& materialData);

    Material* getMaterial(int id);
    int getMaterialID(const std::string& name);
    Material* getRectangleMaterial();
private:
    TextureManager* textureManager;
    EventBus* bus;
    std::unordered_map<int, std::unique_ptr<Material>> idMap;
    std::unordered_map<std::string, int> nameToIDMap;
    int nextID = 0;


};