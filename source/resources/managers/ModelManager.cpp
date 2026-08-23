#include "ModelManager.h"

#include "MeshManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "SkeletonManager.h"

#include "../../resources/loaders/ModelLoader.h"

#include <iostream>

/*namespace
{

    void printMatrix(
        const Mat4& matrix,
        const char* name
    )
    {
        std::cout << name << ":\n";

        std::cout << std::fixed
                  << std::setprecision(4);

        for (int row = 0; row < 4; ++row)
        {
            std::cout
                << "  [ ";

            for (int col = 0; col < 4; ++col)
            {
                std::cout
                    << std::setw(9)
                    << matrix(row, col);

                if (col < 3)
                    std::cout << ", ";
            }

            std::cout << " ]\n";
        }
    }


    void printVector3(
        const Vector3& v,
        const char* name
    )
    {
        std::cout
            << name
            << ": ("
            << v.x << ", "
            << v.y << ", "
            << v.z << ")\n";
    }


    void printQuat(
        const Quat& q,
        const char* name
    )
    {
        std::cout
            << name
            << ": ("
            << q.x << ", "
            << q.y << ", "
            << q.z << ", "
            << q.w << ")\n";
    }

}
*/

ModelManager::ModelManager(
    MeshManager* meshManager,
    MaterialManager* materialManager,
    TextureManager* textureManager,
    SkeletonManager* skeletonManager
)
    : meshManager(meshManager),
      materialManager(materialManager),
      textureManager(textureManager),
      skeletonManager(skeletonManager)
{
}


ModelAssetID ModelManager::loadModel(
    const std::string& name,
    const std::string& path)
{
    if (auto it = lookup.find(name);
        it != lookup.end())
    {
        std::cout
            << "Model already loaded: "
            << name
            << '\n';

        return it->second;
    }


    std::cout
        << "\n========================================\n"
        << "Loading model: "
        << name
        << "\nPath: "
        << path
        << "\n========================================\n";


    ModelLoader loader(path);

    if (!loader.isValid())
    {
        std::cerr
            << "Failed to load model: "
            << path
            << '\n';

        return ModelAssetID{};
    }


    const ModelData& data =
        loader.getModelData();


    ModelAssetDef def;

    def.name = name;
    def.path = path;


    // =========================================================
    // MATERIALS
    // =========================================================

    std::cout
        << "\n--- MATERIALS ---\n";

    std::vector<MaterialID> materialIDs;

    materialIDs.reserve(
        data.materials.size()
    );


    for (const auto& materialData :
         data.materials)
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


        const MaterialTextureInfo* ormInfo =
            nullptr;

        const MaterialTextureInfo* aoInfo =
            nullptr;


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
            const std::string aoPath =
                aoInfo
                    ? aoInfo->path
                    : "";


            TextureID armID =
                textureManager->loadARM(
                    aoPath,
                    ormInfo->path
                );


            if (armID.isValid())
            {
                material.SetTexture(
                    MaterialSlot::ARM,
                    armID
                );
            }
        }


        if (!material.GetTexture(
                MaterialSlot::Albedo).isValid())
        {
            material.SetTexture(
                MaterialSlot::Albedo,
                textureManager->getDefaultAlbedo()
            );
        }


        if (!material.GetTexture(
                MaterialSlot::Normal).isValid())
        {
            material.SetTexture(
                MaterialSlot::Normal,
                textureManager->getDefaultNormal()
            );
        }


        if (!material.GetTexture(
                MaterialSlot::ARM).isValid())
        {
            material.SetTexture(
                MaterialSlot::ARM,
                textureManager->getDefaultWhite()
            );
        }


        if (!material.GetTexture(
                MaterialSlot::Emissive).isValid())
        {
            material.SetTexture(
                MaterialSlot::Emissive,
                textureManager->getDefaultBlack()
            );
        }


        if (!material.GetTexture(
                MaterialSlot::Height).isValid())
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


        materialIDs.push_back(
            materialID
        );
    }

    std::cout
        << "\n========================================\n"
        << "SKELETON\n"
        << "========================================\n";


    if (data.skeleton.has_value())
    {
        const SkeletonData& skeletonData =
            *data.skeleton;


        std::cout
            << "Bone count: "
            << skeletonData.bones.size()
            << "\n\n";


        Skeleton skeleton;

        skeleton.name =
            name + "::Skeleton";

        skeleton.bones.reserve(
            skeletonData.bones.size()
        );


        for (std::size_t i = 0;
             i < skeletonData.bones.size();
             ++i)
        {
            const auto& boneData =
                skeletonData.bones[i];

            Bone bone;

            bone.name =
                boneData.name;

            bone.parentId =
                boneData.parentIndex;

            bone.pos =
                boneData.translation;

            bone.rot =
                boneData.rotation;

            bone.scale =
                boneData.scale;

            bone.invBind =
                boneData.inverseBindMatrix;


            skeleton.bones.push_back(
                std::move(bone)
            );
        }

        def.skeleton =
            skeletonManager->addSkeleton(
                std::make_unique<Skeleton>(
                    std::move(skeleton)
                )
            );


        if (!def.skeleton.isValid())
        {
            std::cerr
                << "Failed to create skeleton for model: "
                << name
                << '\n';

            return ModelAssetID{};
        }


        std::cout
            << "\nSkeleton registered successfully.\n";
    }
    else
    {
        std::cout
            << "Model has NO skeleton.\n";
    }

    def.parts.reserve(
        data.submeshes.size()
    );


    for (std::size_t i = 0;
         i < data.submeshes.size();
         ++i)
    {
        const auto& submesh =
            data.submeshes[i];


        const std::string partMeshName =
            name + "::" + submesh.name;


        ModelPartDef part;

        part.name =
            submesh.name;

        part.parentPartIndex =
            submesh.parentSubmeshIndex;

        part.localPosition =
            submesh.localPosition;

        part.localRotation =
            submesh.localRotation;

        part.localScale =
            submesh.localScale;

        part.attachBoneIndex =
            submesh.attachBoneIndex;

        if (submesh.materialIndex <
            materialIDs.size())
        {
            part.material =
                materialIDs[
                    submesh.materialIndex
                ];
        }



        if (submesh.meshKind ==
            MeshKind::Static)
        {
            const StaticMeshData& meshData =
                data.staticMeshes[
                    submesh.meshIndex
                ];


            StaticMeshID meshID =
                meshManager->addStaticMesh(
                    partMeshName,
                    meshData.vertices,
                    meshData.indices
                );


            if (!meshID.isValid())
            {
                std::cerr
                    << "Failed to create static mesh: "
                    << partMeshName
                    << '\n';

                continue;
            }


            part.mesh =
                meshID;
        }
        else
        {
            const SkinnedMeshData& meshData =
                data.skinnedMeshes[
                    submesh.meshIndex
                ];

        

            SkinnedMeshID meshID =
                meshManager->addSkinnedMesh(
                    partMeshName,
                    meshData.vertices,
                    meshData.indices
                );


            if (!meshID.isValid())
            {
                std::cerr
                    << "Failed to create skinned mesh: "
                    << partMeshName
                    << '\n';

                continue;
            }


            part.mesh =
                meshID;
        }


        def.parts.push_back(
            std::move(part)
        );
    }



    ModelAssetID id =
        pool.insert(
            std::move(def)
        );


    lookup[name] =
        id;


    return id;
}


const ModelAssetDef* ModelManager::getModel(
    ModelAssetID id) const
{
    return pool.get(id);
}


ModelAssetID ModelManager::getModelID(
    const std::string& name) const
{
    auto it =
        lookup.find(name);

    return
        (it != lookup.end())
            ? it->second
            : ModelAssetID{};
}


bool ModelManager::isLoaded(
    const std::string& name) const
{
    return lookup.find(name)
        != lookup.end();
}