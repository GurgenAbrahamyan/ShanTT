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


struct SceneRenderData
{
    struct Batch
    {
        std::vector<Mat4> instances;
    };

    CameraComponent* camera = nullptr;

    TransformComponent* cameraTransform = nullptr;

    std::unordered_map<
        MaterialID,
        std::unordered_map<RenderMesh*, Batch>, MaterialIDHash
    > batches;

    std::unordered_map<
        MaterialID,
        RenderMaterial, MaterialIDHash
    > materials;

    std::vector<GPULight> lights;

    std::vector<ShadowData> shadowData;
    
    
};
