#pragma once

#include "EnTT/entt.hpp"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../data/BatchMap.h"
#include "GPULight.h"
struct RenderContext {

 

    entt::registry* registry = nullptr;

    CameraComponent* camera = nullptr;
    TransformComponent* cameraTransform = nullptr;

    MaterialBatchMap batches;

    std::vector<GPULight> lights;
    std::vector<GPULight*> shadowCasters; 
	int windowWidth = 1280;
    int windowHeight = 720;
  

    int shadowSlots;

    Mat4 lightProjection;
	Mat4 lightView;
};