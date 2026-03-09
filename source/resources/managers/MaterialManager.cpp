#include "MaterialManager.h"
#include "TextureManager.h"
#include "../../resources/assets/Material.h"
#include "../data/MaterialData.h"
#include "../../core/EventBus.h"
#include "../../core/Event.h"
#include "../data/MaterialData.h"

#include <iostream>

MaterialManager::MaterialManager(TextureManager* texMgr, EventBus* bus)
    : textureManager(texMgr), bus(bus) {

    bus->subscribe<InitMaterial>([this](InitMaterial& event) {
        const MaterialData* matData = event.data;
        this->addMaterial(
            *matData
        );
		event.result = this->getMaterial(this->getMaterialID(matData->name));
		});

}

int MaterialManager::addMaterial(const MaterialData& materialData)
{
    const std::string& name = materialData.name;
    const std::vector<MaterialTextureInfo>& textures = materialData.textureInfo;

    if (auto it = nameToIDMap.find(name); it != nameToIDMap.end()) {
        std::cout << "Material already exists: " << name << " (ID: " << it->second << ")\n";
        return it->second;
    }

    int id = nextID++;
    auto mat = std::make_unique<Material>();

    std::cout << "Creating material: " << name << " (ID: " << id << ")\n";
    std::cout << "  Textures provided: " << textures.size() << "\n";

    for (const auto& texInfo : textures)
    {
        if (texInfo.path.empty()) {
            std::cout << "  Texture " << static_cast<int>(texInfo.type) << ": EMPTY (skipped)\n";
            continue;
        }

       
        uint32_t texID = textureManager->addTexture(texInfo.path, texInfo.type);
        if (texID == UINT32_MAX) {
            std::cerr << "  Failed to load texture: " << texInfo.path << "\n";
            continue;
        }

        Texture* tex = textureManager->getTexture(texID);
        if (!tex) {
            std::cerr << "  Retrieved null texture for ID " << texID << "\n";
            continue;
        }

        
        int slot = static_cast<int>(TextureSlot::MAX_SLOTS); // default invalid
        switch (texInfo.type) {
        case TextureType::Albedo:         slot = static_cast<int>(TextureSlot::BASE_COLOR); break;
        case TextureType::Normal:         slot = static_cast<int>(TextureSlot::NORMAL_MAP); break;
        case TextureType::Metallic:  slot = static_cast<int>(TextureSlot::METALLIC_ROUGHNESS); break;
        case TextureType::AO:             slot = static_cast<int>(TextureSlot::OCCLUSION); break;
        case TextureType::Emissive:       slot = static_cast<int>(TextureSlot::EMISSIVE); break;
        }

        if (slot >= 0 && slot < static_cast<int>(TextureSlot::MAX_SLOTS))
        {
            mat->SetTexture(slot, tex);
            std::cout << "  Slot " << slot << " (" << texInfo.path << ") assigned\n";
        }
        else
        {
            std::cerr << "  Unknown texture type for path: " << texInfo.path << "\n";
        }
    }

    
    mat->metallic = materialData.metallic;
    mat->roughness = materialData.roughness;
    mat->ao = materialData.ao;
    mat->setID(id);

    std::cout << "  Properties: Metallic=" << mat->metallic
        << ", Roughness=" << mat->roughness
        << ", AO=" << mat->ao << "\n";

    idMap[id] = std::move(mat);
    nameToIDMap[name] = id;

    std::cout << "Material created successfully!\n";
    return id;
}

Material* MaterialManager::getRectangleMaterial() {
    MaterialData matData;

    matData.name = "Cube_-1";
    MaterialTextureInfo texInfo;
    texInfo.path = "resource/textures/brick_wall.jpg";
    texInfo.type = TextureType::Albedo;
    matData.textureInfo.push_back(texInfo);
    texInfo.path = "resource/textures/brick_wall_specular.png";
    texInfo.type = TextureType::Normal;
    matData.textureInfo.push_back(texInfo);
    return getMaterial(addMaterial(matData));
}


Material* MaterialManager::getMaterial(int id) {
    if (auto it = idMap.find(id); it != idMap.end())
        return it->second.get();

    std::cerr << "Material not found with ID: " << id << "\n";
    return nullptr;
}

int MaterialManager::getMaterialID(const std::string& name) {
    if (auto it = nameToIDMap.find(name); it != nameToIDMap.end())
        return it->second;

    std::cerr << "Material not found with name: " << name << "\n";
    return -1;
}