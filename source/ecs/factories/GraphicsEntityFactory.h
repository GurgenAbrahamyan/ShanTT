#pragma once
#include "../../include/EnTT/entt.hpp"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Quat.h"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/graphics/LightComponent.h"
class CubeMap;
class MeshManager;
class MaterialManager;
enum class LightType;

namespace GraphicsEntityFactory {

    // Primitives only — real models go through ModelManager::loadModel()

    entt::entity createRectangle(
        entt::registry& registry,
        MeshManager& meshManager,
        MaterialManager& textureManager,
        const std::string& tag,
        Vector3 pos = Vector3(0, 0, 0),
        Vector3 scale = Vector3(1, 1, 1)
    );

    entt::entity createLight(
        entt::registry& registry,
        Vector3 position,
		LightComponent type,
        const std::string& tag = "light"
    );

    entt::entity createSkybox(
        entt::registry& registry,
        CubeMap* cubeMap
    );

    entt::entity createCamera(
        entt::registry& registry,
        Vector3 position = Vector3(0, 0, 0),
        CameraComponent camComp = CameraComponent(),
        bool  makeActive = true,
        const std::string& tag = "camera"
    );
}
