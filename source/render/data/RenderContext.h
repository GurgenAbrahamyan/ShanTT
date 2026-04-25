#pragma once

#include "EnTT/entt.hpp"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../data/BatchMap.h"
#include "GPULight.h"
#include "ShadowData.h"
#include "../../ecs/components/graphics/CubeMapComponent.h"
#include "../../resources/assets/Texture.h"
#include "../../resources/managers/ModelManager.h"
struct DebugTexture {
    std::string name;
    unsigned int textureID = 0;
};

struct RenderContext {

 

    entt::registry* registry = nullptr;

    CameraComponent* camera = nullptr;
    TransformComponent* cameraTransform = nullptr;
    CubeMapComponent* cubeMapComp = nullptr;

    MaterialBatchMap batches;

    std::vector<GPULight> lights;
    std::vector<GPULight*> shadowCasters; 
	int windowWidth = 1920;
    int windowHeight = 1200;
  

    int shadowSlots;


    std::vector<ShadowData> shadowData;

    std::vector<DebugTexture> debugTextures;
    Texture* brdfTexture;
	ModelManager* modelManager;
};