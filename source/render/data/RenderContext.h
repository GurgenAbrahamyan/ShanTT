#pragma once

#include "EnTT/entt.hpp"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../data/BatchMap.h"
#include "GPULight.h"
#include "ShadowData.h"
struct RenderContext {

 

    entt::registry* registry = nullptr;

    CameraComponent* camera = nullptr;
    TransformComponent* cameraTransform = nullptr;

    MaterialBatchMap batches;

    std::vector<GPULight> lights;
    std::vector<GPULight*> shadowCasters; 
	int windowWidth = 1920;
    int windowHeight = 1200;
  

    int shadowSlots;

    std::vector<ShadowData> shadowData;
};