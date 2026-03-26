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

    const MaterialTextureInfo* armInfo = nullptr;
    const MaterialTextureInfo* aoInfo = nullptr;

    for (const auto& texInfo : textures)
    {
        if (texInfo.path.empty()) continue;

        int slot = static_cast<int>(TextureSlot::MAX_SLOTS);

        switch (texInfo.type)
        {
        case TextureType::Albedo:
        {
            uint32_t texID = textureManager->addTexture(texInfo.path, texInfo.type);
            Texture* tex = textureManager->getTexture(texID);
            if (tex) mat->SetTexture(static_cast<int>(TextureSlot::BASE_COLOR), tex);
            break;
        }
        case TextureType::Normal:
        {
            uint32_t texID = textureManager->addTexture(texInfo.path, texInfo.type);
            Texture* tex = textureManager->getTexture(texID);
            if (tex) mat->SetTexture(static_cast<int>(TextureSlot::NORMAL_MAP), tex);
            break;
        }
        case TextureType::Emissive:
        {
            uint32_t texID = textureManager->addTexture(texInfo.path, texInfo.type);
            Texture* tex = textureManager->getTexture(texID);
            if (tex) mat->SetTexture(static_cast<int>(TextureSlot::EMISSIVE), tex);
            break;
        }
        case TextureType::Height:
        {
            uint32_t texID = textureManager->addTexture(texInfo.path, texInfo.type);
            Texture* tex = textureManager->getTexture(texID);
            if (tex) mat->SetTexture(static_cast<int>(TextureSlot::HEIGHT), tex);
            break;
        }
        case TextureType::ORM: armInfo = &texInfo; break;
        case TextureType::AO:  aoInfo = &texInfo; break;
        default: break;
        }
    }

    // Pack ARM — always goes through loadARM so AO gets baked into R if present
    if (armInfo) {
        std::string aoPath = aoInfo ? aoInfo->path : "";
        Texture* tex = textureManager->loadARM(aoPath, armInfo->path);
        if (tex)
            mat->SetTexture(static_cast<int>(TextureSlot::ARM), tex);
    }

    // Fallbacks
    if (!mat->GetTexture(static_cast<int>(TextureSlot::BASE_COLOR)))
        mat->SetTexture(static_cast<int>(TextureSlot::BASE_COLOR), textureManager->getDefaultAlbedo());
    if (!mat->GetTexture(static_cast<int>(TextureSlot::NORMAL_MAP)))
        mat->SetTexture(static_cast<int>(TextureSlot::NORMAL_MAP), textureManager->getDefaultNormal());
    if (!mat->GetTexture(static_cast<int>(TextureSlot::ARM)))
        mat->SetTexture(static_cast<int>(TextureSlot::ARM), textureManager->getDefaultWhite());
    if (!mat->GetTexture(static_cast<int>(TextureSlot::EMISSIVE)))
        mat->SetTexture(static_cast<int>(TextureSlot::EMISSIVE), textureManager->getDefaultBlack());
    if (!mat->GetTexture(static_cast<int>(TextureSlot::HEIGHT)))
        mat->SetTexture(static_cast<int>(TextureSlot::HEIGHT), textureManager->getDefaultBlack());

    mat->metallic = materialData.metallic;
    mat->roughness = materialData.roughness;
    mat->ao = materialData.ao;
    mat->baseColorFactor = materialData.baseColorFactor;
    mat->emissiveFactor = materialData.emissiveFactor;
    mat->setID(id);

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