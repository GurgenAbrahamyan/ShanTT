#include "ModelSpawner.h"

#include <iostream>
#include <limits>

#include "resources/managers/ModelManager.h"
#include "../../resources/data/ModelAssetDef.h"

#include "resources/managers/SkeletonManager.h"
#include "resources/assets/Skeleton/Skeleton.h"

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/core/ParentComponent.h"
#include "ecs/components/core/TagComponent.h"

#include "ecs/components/graphics/Renderable.h"
#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/graphics/SocketComponent.h"

#include "ecs/components/physics/BoneComponent.h"

SpawnedModel spawnModel(
    const std::string& name,
    ModelAssetID assetId,
    ModelManager& models,
    SkeletonManager& skeletonManager,
    entt::registry& registry)
{
    SpawnedModel result;

    const ModelAssetDef* def =
        models.getModel(assetId);

    if (!def)
        return result;


    result.root =
        registry.create();

    registry.emplace<TagComponent>(
        result.root,
        name
    );

    if (def->skeleton.isValid())
    {
        SkeletonID skeletonId =
            skeletonManager.instantiate(
                def->skeleton
            );

        registry.emplace<SkeletonComponent>(
            result.root,
            skeletonId
        );

        Skeleton* skeleton =
            skeletonManager.getSkeleton(
                skeletonId
            );

        if (skeleton)
        {
            const size_t boneCount =
                skeleton->bones.size();

            result.sockets.resize(
                boneCount,
                entt::null
            );

            for (uint32_t boneIndex = 0;
                 boneIndex < boneCount;
                 ++boneIndex)
            {
                const Bone& bone =
                    skeleton->bones[boneIndex];


                entt::entity socket =
                    registry.create();


                registry.emplace<TagComponent>(
                    socket,
                    bone.name
                );


                registry.emplace<SocketComponent>(
                    socket
                );


                BoneComponent boneComponent;

                boneComponent.boneIndex =
                    boneIndex;

                registry.emplace<BoneComponent>(
                    socket,
                    boneComponent
                );

                TransformComponent transform;

                transform.position =
                    bone.pos;

                transform.rotation =
                    bone.rot;

                transform.scale =
                    bone.scale;

                registry.emplace<TransformComponent>(
                    socket,
                    transform
                );

    

                ParentComponent parent;

                if (bone.parentId ==
                    std::numeric_limits<uint32_t>::max())
                {
                    parent.parent =
                        result.root;
                }
                else
                {
                    if (bone.parentId >=
                        result.sockets.size())
                    {
                        std::cerr
                            << "ModelSpawner: invalid parent bone "
                            << bone.parentId
                            << " for bone "
                            << bone.name
                            << '\n';

                        parent.parent =
                            result.root;
                    }
                    else
                    {
                        parent.parent =
                            result.sockets[
                                bone.parentId
                            ];
                    }
                }

                registry.emplace<ParentComponent>(
                    socket,
                    parent
                );

                result.sockets[boneIndex] =
                    socket;
            }
        }
        else
        {
            std::cerr
                << "ModelSpawner: failed to obtain skeleton\n";
        }
    }

    std::vector<entt::entity> partEntities;

    partEntities.reserve(
        def->parts.size()
    );

    result.partsByName.reserve(
        def->parts.size()
    );

    for (const auto& part : def->parts)
    {

        entt::entity e =
            registry.create();

        registry.emplace<TagComponent>(
            e,
            part.name
        );

        TransformComponent transform;

        transform.position =
            part.localPosition;

        transform.rotation =
            part.localRotation;

        transform.scale =
            part.localScale;

        registry.emplace<TransformComponent>(
            e,
            transform
        );


        registry.emplace<RenderableComponent>(
            e,
            part.mesh,
            part.material
        );

        entt::entity parentEntity;

        if (part.parentPartIndex ==
            std::numeric_limits<uint32_t>::max())
        {
            parentEntity =
                result.root;
        }
        else
        {
            if (part.parentPartIndex >=
                partEntities.size())
            {
                std::cerr
                    << "ModelSpawner: invalid parent part index "
                    << part.parentPartIndex
                    << " for part "
                    << part.name
                    << '\n';

                parentEntity =
                    result.root;
            }
            else
            {
                parentEntity =
                    partEntities[
                        part.parentPartIndex
                    ];
            }
        }

        ParentComponent parent;

        parent.parent =
            parentEntity;

        registry.emplace<ParentComponent>(
            e,
            parent
        );

        partEntities.push_back(e);

        result.partsByName[
            part.name
        ] = e;
    }

    return result;
}