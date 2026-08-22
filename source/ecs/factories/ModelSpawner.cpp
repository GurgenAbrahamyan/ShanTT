#include "ModelSpawner.h"

#include <iostream>

#include "resources/managers/ModelManager.h"
#include "../../resources/data/ModelAssetDef.h"

#include "ecs/components/core/TransformComponent.h"
#include "ecs/components/graphics/Renderable.h"
#include "ecs/components/graphics/SkeletonComponent.h"
#include "ecs/components/core/ParentComponent.h"
#include "ecs/components/core/TagComponent.h"


SpawnedModel spawnModel(
    const std::string& name,
    ModelAssetID assetId,
    ModelManager& models,
    entt::registry& registry)
{
    SpawnedModel result;

    const ModelAssetDef* def =
        models.getModel(assetId);

    if (!def)
        return result;


    result.root = registry.create();

    registry.emplace<TagComponent>(
        result.root,
        name
    );

    if (def->skeleton.isValid())
    {
        std::cout << "SKELETON IS FUCKING VALID!!!";
        registry.emplace<SkeletonComponent>(
            result.root,
            def->skeleton
        );
    }

    std::vector<entt::entity> partEntities;
    partEntities.reserve(def->parts.size());

    for (const auto& part : def->parts)
    {
        entt::entity e = registry.create();

        registry.emplace<TagComponent>(
            e,
            part.name
        );

        TransformComponent transform;

        transform.position = part.localPosition;
        transform.rotation = part.localRotation;
        transform.scale    = part.localScale;

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

        if (part.parentPartIndex == UINT32_MAX)
        {
            parentEntity = result.root;
        }
        else
        {
            parentEntity =
                partEntities[part.parentPartIndex];
        }

        ParentComponent parent;
        parent.parent = parentEntity;

        registry.emplace<ParentComponent>(
            e,
            parent
        );


        partEntities.push_back(e);

        result.partsByName[part.name] = e;
    }


    return result;
}