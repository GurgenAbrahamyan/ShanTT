#pragma once

#include <vector>
#include <unordered_map>

#include "render/data/GPULight.h"
#include "resources/assets/RenderMesh.h"

#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/core/TransformComponent.h"

#include "../math_custom/Mat4.h"

#include "resources/data/RenderMaterialData.h"
#include "render/data/ShadowData.h"
#include "resources/managers/MaterialHandleTypes.h"
#include "resources/assets/Skeleton/Skeleton.h"

struct SceneRenderData
{
    struct StaticBatch
    {
        std::vector<Mat4> instances;
    };

    struct SkinnedInstance
    {
        Mat4 worldTransform;

        uint32_t paletteOffset = 0;
        
        const Skeleton* skeleton = nullptr;
    };

    struct SkinnedBatch
    {
        std::vector<SkinnedInstance> instances;
    };

    std::vector<Mat4> skinMatrices;


    CameraComponent* camera = nullptr;
    TransformComponent* cameraTransform = nullptr;

    std::unordered_map<
        MaterialID,
        std::unordered_map<StaticMesh*, StaticBatch>,
        MaterialIDHash
    > staticBatches;

    std::unordered_map<
        MaterialID,
        std::unordered_map<SkinnedMesh*, SkinnedBatch>,
        MaterialIDHash
    > skinnedBatches;

    std::unordered_map<
        MaterialID,
        RenderMaterial,
        MaterialIDHash
    > materials;

    std::vector<GPULight> lights;
    std::vector<ShadowData> shadowData;
};