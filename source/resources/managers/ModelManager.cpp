#include "ModelManager.h"

#include "MeshManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"

#include "../../resources/loaders/ModelLoader.h"

#include "../../ecs/components/graphics/ModelComponent.h"

#include <iostream>

ModelManager::ModelManager(
    MeshManager* meshManager,
    MaterialManager* materialManager,
    TextureManager* textureManager
)
    : meshManager(meshManager),
      materialManager(materialManager),
      textureManager(textureManager)
{
}

bool ModelManager::loadModel(
    const std::string& name,
    const std::string& path
)
{
    if (loadedModels.contains(name))
    {
        std::cout
            << "Model already loaded: "
            << name
            << '\n';

        return false;
    }

    ModelLoader loader(path);

    if (!loader.isValid())
    {
        std::cerr
            << "Failed to load model: "
            << path
            << '\n';

        return false;
    }

    const ModelData& data =
        loader.getModelData();

    auto asset =
        std::make_unique<ModelAsset>();

    asset->name = name;
    asset->path = path;

    std::vector<MaterialID> materialIDs;
    materialIDs.reserve(data.materials.size());

    for (const auto& materialData : data.materials)
    {
        Material material;

        material.metallic =
            materialData.metallic;

        material.roughness =
            materialData.roughness;

        material.ao =
            materialData.ao;

        material.heightScale =
            materialData.heightScale;

        material.baseColorFactor =
            materialData.baseColorFactor;

        material.emissiveFactor =
            materialData.emissiveFactor;

        const MaterialTextureInfo* ormInfo = nullptr;
        const MaterialTextureInfo* aoInfo = nullptr;


        for (const auto& textureInfo :
             materialData.textureInfo)
        {
            if (textureInfo.path.empty())
                continue;

            switch (textureInfo.type)
            {
                case TextureType::Albedo:
                {
                    TextureID id =
                        textureManager->addTexture(
                            textureInfo.path,
                            textureInfo.type
                        );

                    if (id.isValid())
                    {
                        material.SetTexture(
                            MaterialSlot::Albedo,
                            id
                        );
                    }

                    break;
                }

                case TextureType::Normal:
                {
                    TextureID id =
                        textureManager->addTexture(
                            textureInfo.path,
                            textureInfo.type
                        );

                    if (id.isValid())
                    {
                        material.SetTexture(
                            MaterialSlot::Normal,
                            id
                        );
                    }

                    break;
                }

                case TextureType::Emissive:
                {
                    TextureID id =
                        textureManager->addTexture(
                            textureInfo.path,
                            textureInfo.type
                        );

                    if (id.isValid())
                    {
                        material.SetTexture(
                            MaterialSlot::Emissive,
                            id
                        );
                    }

                    break;
                }

                case TextureType::Height:
                {
                    TextureID id =
                        textureManager->addTexture(
                            textureInfo.path,
                            textureInfo.type
                        );

                    if (id.isValid())
                    {
                        material.SetTexture(
                            MaterialSlot::Height,
                            id
                        );
                    }

                    break;
                }

                case TextureType::ORM:
                    ormInfo = &textureInfo;
                    break;

                case TextureType::AO:
                    aoInfo = &textureInfo;
                    break;

                case TextureType::Mask:
                case TextureType::Unknown:
                    break;
            }
        }

        if (ormInfo)
        {
            const std::string& armPath =
                ormInfo->path;

            const std::string aoPath =
                aoInfo ? aoInfo->path : "";

            TextureID armID =
                textureManager->loadARM(
                    aoPath,
                    armPath
                );

            if (armID.isValid())
            {
                material.SetTexture(
                    MaterialSlot::ARM,
                    armID
                );
            }
        }

        if (!material
                .GetTexture(MaterialSlot::Albedo)
                .isValid())
        {
            material.SetTexture(
                MaterialSlot::Albedo,
                textureManager->getDefaultAlbedo()
            );
        }

        if (!material
                .GetTexture(MaterialSlot::Normal)
                .isValid())
        {
            material.SetTexture(
                MaterialSlot::Normal,
                textureManager->getDefaultNormal()
            );
        }

        if (!material
                .GetTexture(MaterialSlot::ARM)
                .isValid())
        {
            material.SetTexture(
                MaterialSlot::ARM,
                textureManager->getDefaultWhite()
            );
        }

        if (!material
                .GetTexture(MaterialSlot::Emissive)
                .isValid())
        {
            material.SetTexture(
                MaterialSlot::Emissive,
                textureManager->getDefaultBlack()
            );
        }

        if (!material
                .GetTexture(MaterialSlot::Height)
                .isValid())
        {
            material.SetTexture(
                MaterialSlot::Height,
                textureManager->getDefaultBlack()
            );
        }


        MaterialID materialID =
            materialManager->addMaterial(
                std::move(material),
                name + "::" + materialData.name
            );

        materialIDs.push_back(materialID);
    }

    for (const auto& submesh : data.submeshes)
    {
        const MeshData& meshData =
            data.meshes[submesh.meshIndex];

        const std::string meshName =
            name + "::" + submesh.name;

        MeshID meshID =
            meshManager->addMesh(
                meshName,
                meshData.vertices,
                meshData.indices
            );

        if (!meshID.isValid())
        {
            std::cerr
                << "Failed to create mesh: "
                << meshName
                << '\n';

            continue;
        }

        MeshEntry entry;

        entry.mesh = meshID;

        if (submesh.materialIndex <
            materialIDs.size())
        {
            entry.material =
                materialIDs[submesh.materialIndex];
        }

        entry.localTransform =
            submesh.worldTransform;

        asset->meshes.push_back(entry);
    }



    loadedModels[name] =
        std::move(asset);

    std::cout
        << "Loaded model asset: "
        << name
        << '\n';

    return true;
}

void ModelManager::instantiateModel(
    const std::string& name,
    entt::registry& registry,
    entt::entity entity)
{
    auto it =
        loadedModels.find(name);

    if (it == loadedModels.end())
        return;

    const ModelAsset& asset =
        *it->second;

    auto& model =
        registry.emplace_or_replace<ModelComponent>(
            entity
        );

    model.asset = &asset;
}

bool ModelManager::isLoaded(
    const std::string& name) const
{
    return loadedModels.find(name)
        != loadedModels.end();
}

const std::unordered_map<
    std::string,
    std::unique_ptr<ModelAsset>
>& ModelManager::getLoadedModels() const
{
    return loadedModels;
}